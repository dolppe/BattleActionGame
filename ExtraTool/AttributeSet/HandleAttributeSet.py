import unreal
import sys
import pandas as pd
import os

def insert_df(base: pd.DataFrame, target: pd.DataFrame, idx):
    part1 = base[base.index <= idx]
    part2 = base[base.index > idx]
    total = pd.concat([part1, target])
    total = pd.concat([total, part2])
    total = total.reset_index()
    total = total.drop(["index"], axis=1)

    return total
    


def find_index_array(df : pd.DataFrame , col_name, col_item_name):
    return df.loc[df[col_name] == col_item_name].index

def find_first_index(df : pd.DataFrame,col_name, col_item_name):
    index_array = find_index_array(df, col_name, col_item_name)
    if index_array.empty:
        return -1
    else:
        return index_array[0]
    
def find_last_index(df : pd.DataFrame,col_name, col_item_name):
    index_array = find_index_array(df, col_name, col_item_name)
    if index_array.empty:
        return -1
    else:
        return index_array[-1]


def make_attribute(
    set_name,
    attribute_name,
    base_value =0,
    min_value = 0,
    max_value = 100,
    DerivedAttributeInfo = "",
    CanStack = "FALSE"
):
    data = {
        "AttributeSetName" : set_name,
        "AttributeName" : attribute_name,
        "BaseValue" : base_value,
        "MinValue" : min_value,
        "MaxValue" : max_value,
        "DerivedAttributeInfo" : DerivedAttributeInfo,
        "bCanStack" : CanStack
    }
    return data

def drop_na_and_empty_df_col(df : pd.DataFrame, col_name):
    df = df[df[col_name].str.strip() != ""]
    df = df[df[col_name].notna()]

    return df


def attributeset_data_to_excel(excel_path, attributeset_data : dict):
    excel_df = pd.read_excel(excel_path, "AttributeData")   

    excel_df = drop_na_and_empty_df_col(excel_df, "AttributeSetName")
    excel_df = drop_na_and_empty_df_col(excel_df, "AttributeName")

    for attributeset_key in attributeset_data:
        attributeset_count = (excel_df["AttributeSetName"] == attributeset_key).sum()
        # 존재하지 않는 attributeSet
        if attributeset_count == 0:
            for attribute_info in attributeset_data[attributeset_key]:
                new_attribute_df = pd.DataFrame([make_attribute(attributeset_key, attribute_info.name, attribute_info.base_value)], columns=excel_df.columns)
                excel_df = pd.concat([excel_df, new_attribute_df])
                unreal.log(f"Make attribute: {attributeset_key}-{attribute_info.name}")
        # attribute의 개수가 다른 경우                
        elif attributeset_count != len(attributeset_data[attributeset_key]):
            select_attribute_df = excel_df[excel_df["AttributeSetName"] == attributeset_key]
            for attribute_info in attributeset_data[attributeset_key]:
                exist_df = select_attribute_df[select_attribute_df["AttributeName"] == attribute_info.name]
                if exist_df.empty:
                    new_attribute_df = pd.DataFrame([make_attribute(attributeset_key,attribute_info.name, attribute_info.base_value)], columns=exist_df.columns)
                    last_idx = select_attribute_df.index[-1]
                    excel_df = insert_df(excel_df, new_attribute_df, last_idx)
                    unreal.log(f"Make attribute: {attributeset_key}-{attribute_info.name}")

    excel_df = excel_df.sort_values(by=["AttributeSetName", "AttributeName"])
    
    with pd.ExcelWriter(excel_path, engine="openpyxl", mode="a", if_sheet_exists="replace") as writer:
        excel_df.to_excel(writer, sheet_name="AttributeData", index=False)
    



def get_all_attributeset_in_BattleAbilitySet():
    all_assets = unreal.AssetRegistryHelpers.get_asset_registry().get_all_assets()
    unreal_attributeset_class = set()
    result = dict()

    for asset_data in all_assets:
        if asset_data.asset_class_path.asset_name == "BattleAbilitySet":
            abilityset = unreal.EditorAssetLibrary.load_asset(asset_data.package_name)
            for attribute in abilityset.get_editor_property("granted_attributes"):
                unreal_attributeset_class.add(attribute.get_editor_property("attribute_set"))

    for attributeset_class in unreal_attributeset_class:
        attribute_info_list = unreal.BattleAttributeSetHelper.get_attribute_infos_with_class(attributeset_class)
        result[attributeset_class.get_name()] = attribute_info_list

    return result

# unreal의 attributeset을 가져와서 excel에 최신화함
def write_unreal_attributeset_to_excel(excel_path):
    attributeset_data = get_all_attributeset_in_BattleAbilitySet()
    attributeset_data_to_excel(excel_path, attributeset_data)

def update_character_sheet_attribute(writer : pd.ExcelWriter, path, sheets, attributeset_df, character_type_df):
    
    character_type_names = character_type_df["CharacterTypeName"].unique().tolist()

    for sheet_name in sheets:
        # 이미 시트가 있으니 Attribute만 제대로 있나 체크 후 현 AttributeSetData에 있는 Attribute로 최신화
        if sheet_name in character_type_names:
            character_exist_df = pd.read_excel(path, sheet_name)
            character_exist_attributeset_list = character_exist_df["AttributeSetName"].dropna().unique().tolist()
            character_wanted_attributeset_df = character_type_df[character_type_df["CharacterTypeName"] == sheet_name]        
            character_wanted_attributeset_list = character_wanted_attributeset_df["AttributeSetName"]
            for character_wanted_attributeset in character_wanted_attributeset_list:
                # 이미 해당 attributeset이 존재
                if character_wanted_attributeset in character_exist_attributeset_list:
                    exist_target_df = character_exist_df[character_exist_df["AttributeSetName"] == character_wanted_attributeset]
                    exist_target_list = exist_target_df["AttributeName"].tolist()
                    selected_attributeset_df = attributeset_df[attributeset_df["AttributeSetName"] == character_wanted_attributeset]
                    character_base_df = pd.DataFrame()
                    # attributeset_data에 있는 attribute가 있는지 체크
                    for selected_attribute_name in selected_attributeset_df["AttributeName"]:
                        if selected_attribute_name not in exist_target_list:
                            # 존재하지 않는 경우
                            default_attribute_df = selected_attributeset_df[selected_attributeset_df["AttributeName"] == selected_attribute_name]
                            character_base_df = pd.concat([character_base_df, default_attribute_df])
                    if not character_base_df.empty:
                        idx = find_last_index(character_exist_df, "AttributeSetName",character_wanted_attributeset)
                        character_exist_df = insert_df(character_exist_df, character_base_df, idx)
                else:
                    # 해당 AttributeSet이 없는 것
                    selected_attributeset_df = attributeset_df[attributeset_df["AttributeSetName"] == character_wanted_attributeset]
                    character_exist_df = pd.concat([character_exist_df, selected_attributeset_df])
            character_exist_df = character_exist_df.sort_values(by=["AttributeSetName", "AttributeName"])
            character_exist_df.to_excel(writer, sheet_name=sheet_name, index=False)
            character_type_names.remove(sheet_name)

    return character_type_names


def make_character_attribute_sheet(writer: pd.ExcelWriter, character_type_names, attributeset_df, character_type_df):

    for character_type_name in character_type_names:
        selected_df = character_type_df[character_type_df["CharacterTypeName"] == character_type_name]
        character_attributeset_list = selected_df["AttributeSetName"]
        character_base_df = pd.DataFrame()
        for character_attributeset in character_attributeset_list:
            selected_attributeset_df = attributeset_df[attributeset_df["AttributeSetName"] == character_attributeset]
            character_base_df = pd.concat([character_base_df, selected_attributeset_df])
    
        character_base_df = character_base_df.sort_values(by=["AttributeSetName", "AttributeName"])
        character_base_df.to_excel(writer, sheet_name=character_type_name, index=False)


def write_character_sheet_with_attribute_data_character_data(excel_path):
    attributeset_df = pd.read_excel(excel_path, "AttributeData")
    character_type_df = pd.read_excel(excel_path, "CharacterTypeData")
    sheets = pd.read_excel(excel_path, sheet_name=None)


    with pd.ExcelWriter(excel_path, engine="openpyxl", mode="a", if_sheet_exists="replace") as writer:    
        remain_character_names = update_character_sheet_attribute(writer, excel_path, sheets, attributeset_df=attributeset_df, character_type_df=character_type_df)
        make_character_attribute_sheet(writer, remain_character_names, attributeset_df=attributeset_df, character_type_df=character_type_df)

def get_attributeset_list(attributeset_df: pd.DataFrame):
    unique_list = attributeset_df["AttributeSetName"].dropna().unique().tolist()
    return unique_list

def character_type_data_to_csv(excel_path, output_path):
    sheets = pd.read_excel(excel_path, sheet_name=None)
    character_type_df = pd.read_excel(excel_path, "CharacterTypeData")
    character_type_names = character_type_df["CharacterTypeName"].unique().tolist()

    for sheet_name in sheets:
        if sheet_name in character_type_names:
            target_df = pd.read_excel(excel_path, sheet_name)

            attributeset_list = get_attributeset_list(target_df)

            for attributeset_name in attributeset_list:
                selected_df = target_df[target_df["AttributeSetName"] == attributeset_name]
                selected_df.insert(0, "Attribute", selected_df["AttributeSetName"] + "." + selected_df["AttributeName"])
                selected_df = selected_df.drop(columns=["AttributeSetName", "AttributeName"])
                output_file = output_path + sheet_name + "_" + attributeset_name + ".csv"
                selected_df.to_csv(output_file, index=False)

def attributeset_csv_to_unreal_data_table(csv_path, data_table_path):
    task = unreal.AssetImportTask()
    task.destination_path = data_table_path
    task.replace_existing = True
    task.automated = True
    task.save = True

    csv_factory = unreal.CSVImportFactory()

    all_assets = unreal.AssetRegistryHelpers.get_asset_registry().get_all_assets()

    for asset_data in all_assets:
        if asset_data.asset_class_path.asset_name == "DataTable":
            #unreal.log(asset_data)
            data_table = unreal.EditorAssetLibrary.load_asset(asset_data.package_name)
            row_struct = data_table.get_editor_property("row_struct")
            csv_factory.automated_import_settings.import_row_struct = row_struct
    
    task.factory = csv_factory
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()

    for file in os.listdir(csv_path):
        if file.lower().endswith(".csv"):
            file_path = csv_path + file
            task.filename = file_path
            dest_name = file.rsplit(".",1)[0]
            dest_name = "DT_"+dest_name
            unreal.log(dest_name)
            task.destination_name = dest_name

            asset_tools.import_asset_tasks([task])




excel_path = "F:\\BattleActionGame\\GameDatas\\AttributeSet\\AttributeExcel.xlsx"
csv_output_path = "F:\\BattleActionGame\\GameDatas\\AttributeSet\\Output\\"
data_table_path_base = "F:\\BattleActionGame\\Content\\DataTable\\AttributeSet\\"

unreal.log("HandleAttributeSet Start")
arg = sys.argv

for arg_item in arg:
    # unreal의 AttributeSet을 가져와서 해당 정보를 excel의 AttributeSetData 시트에 최신화
    unreal.log(arg_item)
    if arg_item == "1":
        unreal.log("Start: Unreal AttributeSets To AttributeSetData Sheet")
        attributeset = get_all_attributeset_in_BattleAbilitySet()
        attributeset_data_to_excel(excel_path, attributeset)
    # attributesetData 시트와 CharacterTypeData 시트를 토대로 각 Character 시트를 만들고 최신화
    elif arg_item == "2":
        unreal.log("Start: CharacterTypeData Sheet & AttributeSetData Sheet To CharacterData Sheet")
        write_character_sheet_with_attribute_data_character_data(excel_path)
    # Character 시트를 토대로 csv로 만듦
    elif arg_item == "3":
        unreal.log("Start: CharacterData Sheet To CSV")
        character_type_data_to_csv(excel_path, csv_output_path)
    # csv를 토대로 DataTable을 만듦.
    elif arg_item == "4":
        unreal.log("Start: CSV To Unreal DataTable")
        attributeset_csv_to_unreal_data_table(csv_output_path, data_table_path_base)
        











