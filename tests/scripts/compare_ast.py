import os
import glob
import difflib
import sys

def compare_txt_files():
    # Get the examples directory path relative to current script
    examples_dir = os.path.join(os.path.dirname(__file__), '../../examples/')
    examples_dir = os.path.abspath(examples_dir)
    
    if not os.path.exists(examples_dir):
        print(f"Examples directory not found: {examples_dir}")
        return
    
    # Find all .txt files
    txt_files = glob.glob(os.path.join(examples_dir, '**/*.txt'), recursive=True)
    
    differences_found = False
    
    for txt_file in txt_files:
        # Skip files that already end with _old.txt
        if txt_file.endswith('_old.txt'):
            continue
            
        # Construct the corresponding _old.txt file path
        base_name = txt_file[:-4]  # Remove .txt extension
        old_file = base_name + '_old.txt'
        
        if os.path.exists(old_file):
            print(f"Comparing {txt_file} with {old_file}")
            
            try:
                with open(txt_file, 'r', encoding='utf-8') as f1:
                    content1 = f1.readlines()
                
                with open(old_file, 'r', encoding='utf-8') as f2:
                    content2 = f2.readlines()
                
                # Compare the files
                diff = list(difflib.unified_diff(
                    content2, content1,
                    fromfile=old_file,
                    tofile=txt_file,
                    lineterm=''
                ))
                
                if diff:
                    differences_found = True
                    print(f"Differences found between {txt_file} and {old_file}:")
                    for line in diff:
                        print(line)
                    print("-" * 50)
                    sys.exit(1)
                else:
                    print(f"No differences found between {txt_file} and {old_file}")
                    
            except Exception as e:
                print(f"Error comparing {txt_file} and {old_file}: {e}")
                sys.exit(1)
        else:
            print(f"No corresponding _old.txt file found for {txt_file}")
    
    if not differences_found:
        print("No differences found in any file pairs.")

if __name__ == "__main__":
    compare_txt_files()