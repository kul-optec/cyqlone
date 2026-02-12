# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = "cyqlone"
copyright = "2025, Pieter Pas"
author = "Pieter Pas"
release = "0.0.1"

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.mathjax",
    "sphinx.ext.githubpages",
    "sphinx.ext.doctest",
    "numpydoc",
    "breathe",
]

exclude_patterns = []

autodoc_default_options = {
    "members": None,  # None means True here
    "undoc-members": None,
    "member-order": "bysource",
    "class-doc-from": "class",
    "special-members": "__init__, __call__",
}

breathe_projects = {"cyqlone": "../build/xml"}
breathe_default_project = "cyqlone"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = "sphinx_book_theme"
html_static_path = ["_static"]
html_css_files = ["style.css"]
html_js_files = ["pypi-icon.js"]
html_title = "cyqlone"
html_logo = "images/cyqlone-logo.svg"
html_favicon = "images/cyqlone-logo.svg"
html_context = {
    "github_user": "kul-optec",
    "github_repo": "cyqlone",
    "github_version": "develop",
    "doc_path": "scripts/docs/source",
}
html_show_sourcelink = False
html_theme_options = {
    "use_edit_page_button": True,
    "logo": {
        "text": "cyqlone",
        # "link": "https://github.com/kul-optec/cyqlone",
    },
    "icon_links": [
        {
            "name": "Source code on GitHub",
            "url": "https://github.com/kul-optec/cyqlone",
            "icon": "fa-brands fa-github",
            "type": "fontawesome",
        },
        {
            "name": "Python package on PyPI",
            "url": "https://www.pypi.org/p/cyqlone",
            "icon": "fa-custom fa-pypi",
            "type": "fontawesome",
        },
        {
            "name": "Paper on arXiv",
            "url": "https://arxiv.org/abs/2512.09058",
            "icon": "fa-solid fa-graduation-cap",
            "type": "fontawesome",
        },
    ],
    "pygments_light_style": "vs",
    "pygments_dark_style": "monokai",
    "show_toc_level": 3,
    "show_navbar_depth": 3,
}
