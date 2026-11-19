import os
import re

OUTPUT_DIR="../amalgamation"

SOURCE_FILES = [
    "datastructures/sp_queue.c",
    "utils/sp_utils.c",
    "datastructures/sp_ring_buffer.c",
    "datastructures/cas/mpsc_stream_buffer.c",
    "datastructures/utility/futex.c",
    "datastructures/utility/safe_linear_buffer.c",
    "formats/sp_csvreader.c",
]

HEADER_FILES = [
    "datastructures/sp_queue.h",
    "sp_utils.h",
    "datastructures/utility/futex.h",
    "datastructures/utility/safe_linear_buffer.h",
    "datastructures/sp_ring_buffer.h",
    "datastructures/cas/mpsc_stream_buffer.h",
    "formats/sp_csvreader.h"
]

def generate_header():
    
    generation_disclaimer = []

    generation_disclaimer.append("/* =================================================== */\n")
    generation_disclaimer.append("/* =================================================== */\n")
    generation_disclaimer.append("/* =====  GENERATED FILE - DO NOT EDIT DIRECTLY ====== */\n")
    generation_disclaimer.append("/* =================================================== */\n")
    generation_disclaimer.append("/* =================================================== */\n")
    
    final_header = []
    system_includes = set()
    output_lines = []
    
    for f in HEADER_FILES:
        h_file = "../include/internal/"+f
        with open(h_file, 'r', encoding='utf-8') as f:
            for line in f:
                # system includes are ported, internal includes are skipped, endif are concatenated with a \n
                if line.strip().startswith("#include <"):
                    system_includes.add(line.strip())
                elif not line.strip().startswith('#include "'):
                    output_lines.append(line)
                    if line.strip().startswith("#endif"):
                        output_lines.append("\n")

    for line in generation_disclaimer:
        final_header += line
               
    for i in sorted(list(system_includes)) :
        final_header.append(i+"\n")
    final_header += output_lines
        
    header_file = "seraphic_phoenix_am.h"

    with open(header_file, "w", encoding='utf-8') as f:
        f.writelines(final_header)
    print(header_file,"is generated.")

def generate_source():
    output_lines = []
    output_lines.append('#include "seraphic_phoenix_am.h"\n\n')
    
    for f in SOURCE_FILES:
        src_file = "../src/"+f
        output_lines.append(f"/* --- BEGIN FILE: {src_file} --- */\n")
        with open(src_file, 'r', encoding='utf-8') as f:
            for line in f:
                stripped = line.strip()
                # skipping internal includes since it's all included with *amalgamated_name*.h
                if stripped.startswith('#include "'):
                    continue
                output_lines.append(line)
        output_lines.append(f"\n/* --- END FILE: {src_file} --- */\n\n")
    
    source_file = "seraphic_phoenix_am.c"        

    with open(source_file, "w", encoding='utf-8') as f:
        f.writelines(output_lines)
    print(source_file,"is generated.")

if __name__ == "__main__":
    generate_header()
    generate_source()