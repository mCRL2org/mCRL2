
import sys
import pkg_resources


def check_requirements(requirements_file="../requirements.txt"):
    """Checks that the dependencies listied in the requirements file are installed."""
    with open(requirements_file, encoding="utf-8") as f:
        for requirement in f.readlines():
            if requirement.startswith("#") or requirement.strip() == "":
                continue

            #print(f"Checking {requirement.strip()}")
            pkg_resources.require(requirement)

if __name__ == "__main__":
    try:
        check_requirements()
    except pkg_resources.DistributionNotFound as e:
        print(f"Missing dependency: {e}")
        sys.exit(1)
        
    sys.exit(0)
