$(document).ready(function () {
  $("div.collapse").children().addClass("collapsed");
  $("div.collapse").addClass("collapsed");
  $("div.collapse p.admonition-title").bind("click", function (e) {
    $(this).parent().children().toggleClass("collapsed");
    $(this).parent().toggleClass("collapsed");
  });
});
