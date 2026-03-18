from __future__ import annotations

from docutils import nodes
from docutils.parsers.rst import directives
from sphinx.util.docutils import SphinxDirective


class _Mcrl2AdmonitionDirective(SphinxDirective):
    """Base directive for mCRL2-specific admonitions.

    Usage examples:
      .. mcrl2-definition::
         ...

      .. mcrl2-definition:: Structure graph
         ...

      .. mcrl2-lemma:: 1
         ...
    """

    has_content = True
    optional_arguments = 1
    final_argument_whitespace = True
    option_spec = {
        "class": directives.class_option,
        "name": directives.unchanged,
    }

    # Overridden in subclasses
    label = "Admonition"
    css_class = "mcrl2-admonition"

    def _format_title(self, suffix: str) -> str:
        if suffix.startswith("(") and suffix.endswith(")"):
            return f"{self.label} {suffix}"
        return f"{self.label} ({suffix})"

    def run(self):
        self.assert_has_content()

        title = self.label
        if self.arguments:
            suffix = self.arguments[0].strip()
            if suffix:
                title = self._format_title(suffix)

        admonition = nodes.admonition()
        admonition["classes"].extend(["mcrl2-admonition", self.css_class])
        if "class" in self.options:
            admonition["classes"].extend(self.options["class"])
        admonition += nodes.title(text=title)
        self.add_name(admonition)

        self.state.nested_parse(self.content, self.content_offset, admonition)
        return [admonition]


class Mcrl2DefinitionDirective(_Mcrl2AdmonitionDirective):
    label = "Definition"
    css_class = "mcrl2-definition"


class Mcrl2LemmaDirective(_Mcrl2AdmonitionDirective):
    label = "Lemma"
    css_class = "mcrl2-lemma"


class Mcrl2ProofDirective(_Mcrl2AdmonitionDirective):
    label = "Proof"
    css_class = "mcrl2-proof"


class Mcrl2ExampleDirective(_Mcrl2AdmonitionDirective):
    label = "Example"
    css_class = "mcrl2-example"


class Mcrl2ExerciseDirective(_Mcrl2AdmonitionDirective):
    label = "Exercise"
    css_class = "mcrl2-exercise"


class Mcrl2ExercisesDirective(_Mcrl2AdmonitionDirective):
    label = "Exercises"
    css_class = "mcrl2-exercises"


class Mcrl2SolutionDirective(_Mcrl2AdmonitionDirective):
    label = "Solution"
    css_class = "mcrl2-solution"


class Mcrl2RemarkDirective(_Mcrl2AdmonitionDirective):
    label = "Remark"
    css_class = "mcrl2-remark"


class Mcrl2ReleaseDirective(_Mcrl2AdmonitionDirective):
    label = "Release"
    css_class = "mcrl2-release"


class Mcrl2DisclaimerDirective(_Mcrl2AdmonitionDirective):
    label = "Disclaimer"
    css_class = "mcrl2-disclaimer"


def setup(app):
    app.add_directive("definition", Mcrl2DefinitionDirective)
    app.add_directive("lemma", Mcrl2LemmaDirective)
    app.add_directive("proof", Mcrl2ProofDirective)
    app.add_directive("example", Mcrl2ExampleDirective)
    app.add_directive("exercise", Mcrl2ExerciseDirective)
    app.add_directive("exercises", Mcrl2ExercisesDirective)
    app.add_directive("solution", Mcrl2SolutionDirective)
    app.add_directive("remark", Mcrl2RemarkDirective)
    app.add_directive("release", Mcrl2ReleaseDirective)
    app.add_directive("disclaimer", Mcrl2DisclaimerDirective)

    return {"parallel_read_safe": True, "parallel_write_safe": True}
