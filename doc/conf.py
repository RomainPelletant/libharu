# SPDX-License-Identifier: Apache-2.0
"""Standalone Sphinx documentation, based on the Zephyr application template."""
import re
from pathlib import Path

project = "Libharu on Zephyr"
author = "Libharu contributors"
copyright = "2026, Libharu contributors"
version_header = (Path(__file__).resolve().parents[1] / "include/hpdf_version.h").read_text()
release = ".".join(re.search(r"#define HPDF_" + part + r"_VERSION (\d+)",
                            version_header).group(1)
                   for part in ("MAJOR", "MINOR", "BUGFIX"))
version = release
extensions = ["sphinx.ext.intersphinx"]
exclude_patterns = ["_build_sphinx", "_build_doxygen", "_doxygen", "Thumbs.db", ".DS_Store"]
html_theme = "sphinx_rtd_theme"
html_title = f"{project} {release}"
html_static_path = ["_static"]
html_css_files = ["custom.css"]
intersphinx_mapping = {"zephyr": ("https://docs.zephyrproject.org/latest/", None)}
