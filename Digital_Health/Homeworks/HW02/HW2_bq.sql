WITH icd_data AS (
  SELECT DISTINCT 
    icd.subject_id, 
    icd.hadm_id, 
    icd.icd_version, 
    icd.icd_code, 

  FROM
    `physionet-data.mimiciv_hosp.diagnoses_icd` as icd
    
  WHERE
    (
      (icd.icd_version = 9 AND icd.icd_code LIKE "4151%") OR 
      (icd.icd_version = 10 AND icd.icd_code LIKE "I26%")
    )
),

age_data AS (
  SELECT DISTINCT
    derived_age.subject_id, 
    derived_age.hadm_id,
    derived_age.age, 
  
  FROM
    `physionet-data.mimiciv_2_2_derived.age` as derived_age
  
  INNER JOIN `physionet-data.mimiciv_hosp.patients` as patient
    ON derived_age.subject_id = patient.subject_id
  
  WHERE
    derived_age.age >= 21
), 

patient_id AS(
  SELECT DISTINCT
  icu.subject_id, 
  icu.hadm_id, 
  icu.stay_id, 
  RANK()
      OVER(
        PARTITION BY icu.subject_id 
        ORDER BY icu.icu_intime DESC
      ) AS _rank
  FROM
    `physionet-data.mimiciv_2_2_derived.icustay_detail` as icu
  
  INNER JOIN icd_data
    ON icd_data.subject_id = icu.subject_id AND icd_data.hadm_id = icu.hadm_id

  INNER JOIN age_data
    ON age_data.subject_id = icu.subject_id and age_data.hadm_id = icu.hadm_id
),

patient_data AS (
  SELECT DISTINCT
    icu.subject_id, 
    icu.hadm_id, 
    icu.stay_id, 
    icu.hospital_expire_flag AS in_hospital_mortality, 
    age_data.age,
    icu.gender,
    icu.admittime,
    icu.race,

    RANK()
      OVER(
        PARTITION BY icu.subject_id 
        ORDER BY icu.icu_intime DESC
      ) AS _rank
  
  FROM
    `physionet-data.mimiciv_2_2_derived.icustay_detail` as icu
  
  INNER JOIN icd_data
    ON icd_data.subject_id = icu.subject_id AND icd_data.hadm_id = icu.hadm_id

  INNER JOIN age_data
    ON age_data.subject_id = icu.subject_id and age_data.hadm_id = icu.hadm_id
),

Urine_data AS (SELECT DISTINCT
-- o.itemid,
-- d.label,
p.subject_id,
p.hadm_id,
p.stay_id,
SUM(SAFE_CAST(o.value AS FLOAT64)) AS total_urine_output_ml,
-- o.valueuom,
-- o.charttime,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.outputevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE '%Urine%'
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id--,o.itemid
),

Hemoglobin_data AS (
SELECT
p.subject_id,
p.hadm_id,
p.stay_id,
MIN(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 100 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS min_hemoglobin,
MAX(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 100 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS max_hemoglobin,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.chartevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Hemoglobin'
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id--,o.itemid
),

Heart_rate_data AS (
SELECT
p.subject_id,
p.hadm_id,
p.stay_id,
MAX(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 20 
         AND SAFE_CAST(o.value AS FLOAT64) < 300 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS max_heart_rate,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.chartevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Heart Rate'
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id
-- ORDER BY max_heart_rate ASC
),

Respiratory_rate_data AS (
SELECT
p.subject_id,
p.hadm_id,
p.stay_id,
MIN(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS min_resp_rate,
MAX(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS max_resp_rate,
AVG(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS avg_resp_rate,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.chartevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Respiratory Rate' AND d.itemid = 220210
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id
),

Glucose_s_data AS (
SELECT
p.subject_id,
p.hadm_id,
p.stay_id,
MIN(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS min_glucose_s,
MAX(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS max_glucose_s,
AVG(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS avg_glucose_s,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.chartevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Glucose (serum)' AND o.itemid = 220621
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id
-- ORDER BY p.subject_id ASC
),

Glucose_wb_data AS (
SELECT
p.subject_id,
p.hadm_id,
p.stay_id,
MIN(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS min_glucose_wb,
MAX(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS max_glucose_wb,
AVG(CASE 
        WHEN SAFE_CAST(o.value AS FLOAT64) > 0 
         AND SAFE_CAST(o.value AS FLOAT64) < 1000 
        THEN SAFE_CAST(o.value AS FLOAT64) 
        ELSE NULL 
      END) AS avg_glucose_wb,
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.chartevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.charttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Glucose (whole blood)' AND d.itemid = 226537
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id
-- ORDER BY p.subject_id ASC
),

Glucose_data AS(
SELECT
  p.subject_id,
  p.hadm_id,
  p.stay_id,
  COALESCE((s.avg_glucose_s + wb.avg_glucose_wb) / 2.0, s.avg_glucose_s, wb.avg_glucose_wb) AS combined_avg_glucose,
  COALESCE((s.max_glucose_s + wb.max_glucose_wb) / 2.0, s.max_glucose_s, wb.max_glucose_wb) AS combined_max_glucose,
  COALESCE((s.min_glucose_s + wb.min_glucose_wb) / 2.0, s.min_glucose_s, wb.min_glucose_wb) AS combined_min_glucose
FROM patient_data AS p
LEFT JOIN Glucose_s_data AS s
  ON s.subject_id = p.subject_id AND s.hadm_id = p.hadm_id  AND s.stay_id = p.stay_id
LEFT JOIN Glucose_wb_data AS wb
  ON p.subject_id = wb.subject_id AND p.hadm_id = wb.hadm_id  AND p.stay_id = wb.stay_id
WHERE p._rank = 1 
-- GROUP BY p.subject_id,p.hadm_id,p.stay_id
),

Vasopressin_data AS (
SELECT DISTINCT
  p.subject_id,
  p.hadm_id,
  p.stay_id,
  MIN(o.amount) AS vasopressin
FROM patient_data as p
LEFT JOIN `physionet-data.mimiciv_2_2_icu.inputevents` as o
  ON p.subject_id = o.subject_id AND p.hadm_id = o.hadm_id  AND p.stay_id = o.stay_id AND o.starttime BETWEEN TIMESTAMP_SUB(p.admittime, INTERVAL 6 HOUR) AND TIMESTAMP_ADD(p.admittime, INTERVAL 24 HOUR)

LEFT JOIN `physionet-data.mimiciv_2_2_icu.d_items` as d
ON o.itemid = d.itemid AND d.label LIKE 'Vasopressin'
WHERE p._rank = 1 
GROUP BY p.subject_id,p.hadm_id,p.stay_id--,o.itemid
)

-- SELECT 
-- *
-- FROM 
-- -- patient_data
-- -- Urine_data
-- -- Hemoglobin_data
-- -- Heart_rate_data
-- -- Respiratory_rate_data
-- Glucose_data
-- -- Vasopressin_data
-- -- WHERE _rank=1
SELECT
  p.subject_id,
  p.hadm_id,
  p.stay_id,
  p.in_hospital_mortality, 
  p.age,
  p.gender,
  uri.total_urine_output_ml,
  hemo.min_hemoglobin,
  hemo.max_hemoglobin,
  heart.max_heart_rate,
  resp.min_resp_rate,
  resp.avg_resp_rate,
  resp.max_resp_rate,
  glu.combined_min_glucose,
  glu.combined_avg_glucose,
  glu.combined_max_glucose,
  vd.vasopressin -- null is not use vasopressin
FROM patient_data AS p
LEFT JOIN Urine_data AS uri
  ON p.subject_id = uri.subject_id AND p.hadm_id = uri.hadm_id  AND p.stay_id = uri.stay_id
LEFT JOIN Hemoglobin_data AS hemo
  ON p.subject_id = hemo.subject_id AND p.hadm_id = hemo.hadm_id  AND p.stay_id = hemo.stay_id
LEFT JOIN Heart_rate_data AS heart
  ON p.subject_id = heart.subject_id AND p.hadm_id = heart.hadm_id  AND p.stay_id = heart.stay_id
LEFT JOIN Respiratory_rate_data AS resp
  ON p.subject_id = resp.subject_id AND p.hadm_id = resp.hadm_id  AND p.stay_id = resp.stay_id
LEFT JOIN Glucose_data AS glu
  ON p.subject_id = glu.subject_id AND p.hadm_id = glu.hadm_id  AND p.stay_id = glu.stay_id
LEFT JOIN Vasopressin_data AS vd
  ON p.subject_id = vd.subject_id AND p.hadm_id = vd.hadm_id  AND p.stay_id = vd.stay_id
WHERE p._rank = 1

