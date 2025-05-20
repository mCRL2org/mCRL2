import sys

import importlib.metadata as metadata
from packaging.requirements import Requirement


def check_requirements(requirements_file="../requirements.txt"):
    """Checks that the dependencies listed in the requirements file are installed and of sufficient version."""
    with open(requirements_file, encoding="utf-8") as f:
        for requirement in f.readlines():
            if requirement.startswith("#") or requirement.strip() == "":
                continue

            #print(f"Checking {requirement.strip()}")
            req = Requirement(requirement.strip())
            try:
                installed_version = metadata.version(req.name)
                if not req.specifier.contains(installed_version):
                    raise ValueError(f"Package {req.name} version {installed_version} does not satisfy {req.specifier}")
            except metadata.PackageNotFoundError as e:
                raise RuntimeError(f"Package {req.name} not found") from e

if __name__ == "__main__":
    try:
        check_requirements()
    except (metadata.PackageNotFoundError, ValueError, RuntimeError) as e:
        print(f"Unsatisfied Python dependency: {e}")
        sys.exit(1)
        
    sys.exit(0)
