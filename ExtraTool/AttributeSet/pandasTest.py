import pandas as pd

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

def get_attributeset_list(attributeset_df: pd.DataFrame):
    unique_list = attributeset_df["AttributeSetName"].dropna().unique().tolist()
    return unique_list

def drop_na_and_empty_df_col(df : pd.DataFrame, col_name):
    df = df[df[col_name].str.strip() != ""]
    df = df[df[col_name].notna()]

    return df

def update_character_sheet_attribute(writer : pd.ExcelWriter, sheets, attributeset_df, character_type_df):
    
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
        remain_character_names = update_character_sheet_attribute(writer, sheets, attributeset_df=attributeset_df, character_type_df=character_type_df)
        make_character_attribute_sheet(writer, remain_character_names, attributeset_df=attributeset_df, character_type_df=character_type_df)


def character_type_data_to_csv(excel_path, output_path):
    sheets = pd.read_excel(excel_path, sheet_name=None)
    character_type_df = pd.read_excel(path, "CharacterTypeData")
    character_type_names = character_type_df["CharacterTypeName"].unique().tolist()

    for sheet_name in sheets:
        if sheet_name in character_type_names:
            target_df = pd.read_excel(excel_path, sheet_name)

            attributeset_list = get_attributeset_list(target_df)

            for attributeset_name in attributeset_list:
                selected_df = target_df[target_df["AttributeSetName"] == attributeset_name]
                selected_df.insert(0, "Attribute", selected_df["AttributeSetName"] + "." + selected_df["AttributeName"])
                selected_df = selected_df.drop(columns=["AttributeSetName", "AttributeName"])
                output_file = output_path + sheet_name + attributeset_name + ".csv"
                selected_df.to_csv(output_file, index=False)



path = "F:\\BattleActionGame\\GameDatas\\AttributeExcelTest.xlsx"
output = "F:\\BattleActionGame\\GameDatas\\Output\\"

#write_character_sheet_with_attribute_data_character_data(path)
#character_type_data_to_csv(path, output)


number = input("숫자를 입력하세요: ")


#write_character_sheet_with_attribute_data_character_data(path)




'''

attributeset_df = pd.read_excel(path, "AttributeData")
character_type_df = pd.read_excel(path, "CharacterTypeData")
sheets = pd.read_excel(path, sheet_name=None)

character_type_names = character_type_df["CharacterTypeName"].unique().tolist()
with pd.ExcelWriter(path, engine="openpyxl", mode="a", if_sheet_exists="replace") as writer:
    for sheet_name in sheets:
        # 이미 시트가 있으니 Attribute만 제대로 있나 체크 후 현 AttributeSetData에 있는 Attribute로 최신화
        if sheet_name in character_type_names:
            character_exist_df = pd.read_excel(path, sheet_name)
            character_exist_attributeset_list = character_exist_df["AttributeSetName"].dropna().unique().tolist()
            character_wanted_attributeset_df = character_type_df[character_type_df["CharacterTypeName"] == sheet_name]        
            character_wanted_attributeset_list = character_wanted_attributeset_df["AttributeSetName"]
            # Character_type_df의 AttributeSet
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


    for character_type_name in character_type_names:
        selected_df = character_type_df[character_type_df["CharacterTypeName"] == character_type_name]
        character_attributeset_list = selected_df["AttributeSetName"]
        character_base_df = pd.DataFrame()
        for character_attributeset in character_attributeset_list:
            selected_attributeset_df = attributeset_df[attributeset_df["AttributeSetName"] == character_attributeset]
            character_base_df = pd.concat([character_base_df, selected_attributeset_df])
        
        character_base_df = character_base_df.sort_values(by=["AttributeSetName", "AttributeName"])
        character_base_df.to_excel(writer, sheet_name=character_type_name, index=False)
    


        

'''







'''
task = unreal.AssetImportTask()
task.destination_path = data_table_path_base
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

for file in os.listdir(csv_folder_path):
    if file.lower().endswith(".csv"):
        file_path = csv_folder_path + file
        task.filename = file_path
        dest_name = file.rsplit(".",1)[0]
        dest_name = "DT_"+dest_name
        unreal.log(dest_name)
        task.destination_name = file

        asset_tools.import_asset_tasks([task])


'''

















'''
all_assets = unreal.AssetRegistryHelpers.get_asset_registry().get_all_assets()
unreal_attributeset_class = set()
result = dict()

for asset_data in all_assets:
    if asset_data.asset_class_path.asset_name == "DataTable":
        #unreal.log(asset_data)
        data_table = unreal.EditorAssetLibrary.load_asset(asset_data.package_name)
        row_struct = data_table.get_editor_property("row_struct")

        if row_struct.get_name() == "AttributeMetaData":
            unreal.log("ASDF")



'''























''' attributeset Test
unreal_attribute_data = dict()

unreal_attribute_data["BattleCombatSet"] = {"AttackPower", "ImpactResistance", "ImpactDamage"}
unreal_attribute_data["BattleHealthSet"] = {"Health", "MaxHealth", "Stamina", "MaxStamina"}
unreal_attribute_data["BattleHealthSe1t"] = {"Health", "MaxHealth", "Stamina", "MaxStamina"}
columns = ["AttributeSetName", "AttributeName", "DefaultBaseValue", "DefaultMinValue", "DefaultMaxValue","DerivedAttributeInfo", "bCanStack"]

excel_df = pd.read_excel(path, "AttributeData")

for attributeset_key in unreal_attribute_data:
    target_count = (excel_df["AttributeSetName"] == attributeset_key).sum()
    if target_count == 0:
        for attribute_name in unreal_attribute_data[attributeset_key]:
            attribute_df = pd.DataFrame([make_attribute(attributeset_key, attribute_name)], columns=excel_df.columns)
            excel_df = pd.concat([excel_df, attribute_df])
    elif target_count != len(unreal_attribute_data[attributeset_key]):
        target_attribute_df = excel_df[excel_df["AttributeSetName"] == attributeset_key]
        print(target_attribute_df)
        for attribute_name in unreal_attribute_data[attributeset_key]:
            exist_df = target_attribute_df[target_attribute_df["AttributeName"] == attribute_name]
            if exist_df.empty:
                attribute_df = pd.DataFrame([make_attribute(attributeset_key, attribute_name)], columns=exist_df.columns)
                last_idx = target_attribute_df.index[-1]
                excel_df = insert_df(excel_df, attribute_df,last_idx)
       


excel_df.to_excel(path,sheet_name="AttributeData",index=False)




'''



