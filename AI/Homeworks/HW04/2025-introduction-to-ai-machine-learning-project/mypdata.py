import pandas as pd
from sklearn.model_selection import train_test_split
from imblearn.over_sampling import SMOTE

# df = pd.read_csv("train.csv")

def get_train_xy(data):
    df = pd.read_csv(data)
    if "index" in df.columns:
        df = df.drop("index",axis=1)
    df = df[df["0"] < 100].reset_index(drop=True)
    X, y = df.drop(["target"], axis=1), df["target"]
    return X,y

def resample_xy(X,y): 
    smote = SMOTE(random_state=42)
    X_resampled, y_resampled = smote.fit_resample(X, y)
    return X_resampled, y_resampled

def get_split(X_resampled,y_resampled):
    X_train, X_test, y_train, y_test = train_test_split(X_resampled, y_resampled, test_size=0.2, random_state=42)
    return X_train, X_test, y_train, y_test

def get_c_s_data(data):
    # 先取得所有欄位
    all_columns = data.columns.tolist()

    # 轉換為 0-based index
    structure_indices = list(range(36, 96))
    structure_columns = [str(i) for i in range(36, 96)]#[all_columns[i] for i in structure_indices]
    print("list2=",structure_columns)
    # 確保 target 存在於欄位中
    target_column = 'target'  # 若實際名稱不同，請替換
    if target_column not in all_columns:
        structure_group = data[structure_columns]

        # 結構資料欄位：從所有欄位中移除連續欄位（但保留 target）
        continuous_columns = [col for col in all_columns if col not in structure_columns and col != target_column and col!="index"]
        continuous_group = data[continuous_columns]
    else:
        # 加上 target 欄
        structure_group = data[structure_columns + [target_column]]

        # 結構資料欄位：從所有欄位中移除連續欄位（但保留 target）
        continuous_columns = [col for col in all_columns if col not in structure_columns and col != target_column and col!="index"]
        continuous_group = data[continuous_columns + [target_column]]
    return structure_group,continuous_group