from pathlib import Path
import importlib.util


ROOT_GENERATOR = Path(__file__).resolve().parents[1] / "make_hlp_sources.py"
spec = importlib.util.spec_from_file_location("reversi_hlp_generator", ROOT_GENERATOR)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


if __name__ == "__main__":
    module.main(["--locale", "zh-CN"])
