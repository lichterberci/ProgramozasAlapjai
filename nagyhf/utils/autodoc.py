import os
import sys
import re
from typing import List, Dict, Union, Tuple
from argparse import ArgumentError
from dataclasses import dataclass

@dataclass
class FunctionDocs :
    
    comment_brief: str
    comment_params: List[Tuple[str, str]]
    comment_returns: str
    function_string: str
    function_name: str
    
    def __init__(self):
        self.comment_brief = ""
        self.comment_params = []
        self.comment_returns = ""
        self.function_string = ""
        self.function_name = ""
        
def read_files_in_dir (path: str, extensions: List[str]):
    
    if os.path.isdir(path) == False :
        raise ArgumentError (None, f"{path} is not a directory!")
    
    result = []
    
    files = os.listdir(path)

    for file in files :
                
        if os.path.isfile(os.path.join(path, file)) == False :
            continue
                
        if (os.path.splitext(os.path.join(path, file))[1] in extensions) == False :
            continue
                
        with open(os.path.join(path, file), 'r') as f :
            contents = f.readlines()
        
        result.append({
            "filename" : file,
            "contents" : contents
        })

    return result

def create_function_docs (filename: str, contents: List[str]) :
    """
    returns: a piece of the markdown text
    """
    
    is_reading_comment: bool = False
    is_reading_function: bool = False
    comment_state: str = "NONE"
    
    output_list: List[FunctionDocs] = [] # the result of all the functions' output
        
    temp: FunctionDocs = FunctionDocs()
    
    for line in contents :
                
        if is_reading_comment == False and re.search(r"^\s*///(\s+|@param|@brief|@returns)", line) != None :
            is_reading_comment = True
            is_reading_function = False
            temp = FunctionDocs()
        
        if is_reading_comment and re.search(r"^\s*///(\s+|@param|@brief|@returns)", line) == None :
            is_reading_comment = False
            is_reading_function = True
        
        if is_reading_comment :
            if re.search(r"^\s*///\s*@brief", line) != None :
                comment_state = "BRIEF"
                
                temp.comment_brief += re.split(r"^\s*///\s*@brief", line)[1]
                
            elif re.search(r"^\s*///\s*@param", line) != None :
                comment_state = "PARAM"
                
                param_name_and_comment: str = re.split(r"^\s*///\s*@param", line)[1].strip()
                                
                words = re.split(r"\s+", param_name_and_comment)
                
                temp.comment_params.append((words[0], re.sub(r"^(\S)+", "", param_name_and_comment)))
                
            elif re.search(r"^\s*///\s*@returns", line) != None :
                comment_state = "RETURN"
                
                temp.comment_returns += re.split(r"^\s*///\s*@returns", line)[1]

            else :
                # just append the line at the end of the string, dictated by comment_state
                
                if comment_state == "BRIEF" :
                    temp.comment_brief += line.split("///")[1]
                elif comment_state == "PARAM" :
                    # the comment part of the last item
                    current_param = temp.comment_params[-1]
                    param_name, param_comment = current_param
                    param_comment += line.split("///")[1]
                    temp.comment_params[-1] = (param_name, param_comment)
                elif comment_state == "RETURN" :
                    temp.comment_returns += line.split("///")[1]
        
        if is_reading_function :
            
            # if there is no real value, we don't wanna read it
            if re.search(r"^\s*(;|{)", line) != None :
                is_reading_comment = False
                is_reading_function = False
                output_list.append(temp)
                
                # calculate function name
                
                match = re.search(r"^\s*(\w|\*)+\s+\w+(\(|\s)", temp.function_string)
                
                if match != None :
                    function_name = match.group(0).split()[1]
                    
                    function_name = function_name.replace("(", "")
                    
                    temp.function_name = function_name.strip()
            
            temp.function_string += line
            
            if re.search(r"(;|{)", line) != None :
                is_reading_comment = False
                is_reading_function = False
                
                # calculate function name
                
                match = re.search(r"^\s*(\w|\*)+\s+\w+(\(|\s)", temp.function_string)
                
                if match != None :
                    function_name = match.group(0).split()[1]
                    
                    function_name = function_name.replace("(", "")
                    
                    temp.function_name = function_name.strip()
                    
                output_list.append(temp)
        
    
    return output_list

def create_markdown_text_for_file (filename: str, function_docs_list: List[FunctionDocs]) :
    
    result = ""
    
    result += f"## __{filename}__\n"
    
    for docs in function_docs_list :
        
        result += f"### __{docs.function_name}__\n"
        
        result += "```c\n"
        result += docs.function_string.strip() + "\n"
        result += "```\n"
        
        if docs.comment_brief != "" :
            result += "__Summary:__\n"
            result += docs.comment_brief + "\n"
        
        for param in docs.comment_params :
            param_name, param_comment = param
            
            if param_comment != "" :
                result += f"- __{param_name}__: {param_comment}\n"
                
        result += "\n"
                
        if docs.comment_returns != "" :
            result += "__Returns:__\n"
            result += docs.comment_returns + "\n"
            
        result += "\n\n"
            
    return result

def main (argc: int, args: List[str]):

    if argc > 4 or argc < 3 :
        print("ERROR: argument count incorrect! (should be 2 or 3!)")
        return
    
    append_file_name = ""
    output_file_name = ""
    search_dir = ""
    
    if argc == 3 :
        # codefilepath, dir, outputfilepath
        search_dir = args[1]
        output_file_name = args[2]
    elif argc == 4 :
        # outputfilepath, appendfilepath, dir, outputfilepath
        append_file_name = args[1]
        search_dir = args[2]
        output_file_name = args[3]

    file_data_list = read_files_in_dir(search_dir, [".h", ".c"])

    result_markdown = ""
    
    result_markdown += f"# __API documentation__\n\n\n"

    documentations_of_files = []

    for file_data in file_data_list :
                
        function_docs = create_function_docs(file_data["filename"], file_data["contents"])
        
        documentation_of_file = create_markdown_text_for_file(file_data["filename"], function_docs)
        
        documentations_of_files.append(documentation_of_file)
    
    result_markdown += "\n\n---\n\n".join(documentations_of_files)

    append_file_contents = []
    
    if append_file_name != "" :
        with open (os.path.join(search_dir, append_file_name), "r") as f :
            append_file_contents = f.readlines()

    with open(os.path.join(search_dir, output_file_name), "w") as f :
        f.writelines(append_file_contents)
        f.write(result_markdown)
        
    print(f"Result saved to {os.path.join(search_dir, output_file_name)}")
    
    print("code exited safely")

if __name__ == "__main__" :
    
    main (len(sys.argv), sys.argv)