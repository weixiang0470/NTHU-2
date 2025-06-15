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
    `physionet-data.mimiciv_derived.age` as derived_age
  
  INNER JOIN `physionet-data.mimiciv_hosp.patients` as patient
    ON derived_age.subject_id = patient.subject_id
  
  WHERE
    derived_age.age >= 21
), 

patient_data AS (
  SELECT DISTINCT
    icu.subject_id, 
    icu.hadm_id, 
    icu.stay_id, 
    icu.hospital_expire_flag AS in_hospital_mortality, 

    RANK()
      OVER(
        PARTITION BY icu.subject_id 
        ORDER BY icu.icu_intime DESC
      ) AS _rank
  
  FROM
    `physionet-data.mimiciv_derived.icustay_detail` as icu
  
  INNER JOIN icd_data
    ON icd_data.subject_id = icu.subject_id AND icd_data.hadm_id = icu.hadm_id

  INNER JOIN age_data
    ON age_data.subject_id = icu.subject_id and age_data.hadm_id = icu.hadm_id
)


SELECT
  * EXCEPT(_rank)

FROM
  patient_data

WHERE
  _rank = 1