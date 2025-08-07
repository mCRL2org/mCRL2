import subprocess
import sys
import os

# Change working dir to the script path
os.chdir(os.path.dirname(os.path.abspath(__file__)))

script_dir = os.path.dirname(__file__)

subprocess.run([sys.executable, os.path.join(script_dir, 'generate_classes.py')], check=True)
subprocess.run([sys.executable, os.path.join(script_dir, 'generate_data_types.py')], check=True)
subprocess.run([sys.executable, os.path.join(script_dir, 'generate_template_overloads.py')], check=True)
subprocess.run([sys.executable, os.path.join(script_dir, 'generate_term_functions.py')], check=True)
subprocess.run([sys.executable, os.path.join(script_dir, 'generate_traverser_functions.py')], check=True)
subprocess.run([sys.executable, os.path.join(script_dir, 'generate_traversers.py')], check=True)
