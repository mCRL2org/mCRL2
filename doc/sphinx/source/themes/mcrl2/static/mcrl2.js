$(document).ready(function () {
  $("div.collapse p.admonition-title").bind("click", function (e) {
    $(this).parent().toggleClass("visible");
  });
});
