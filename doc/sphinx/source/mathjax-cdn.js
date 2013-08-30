function getParams()
{
  var scripts = document.getElementsByTagName('script');
  var params = scripts[scripts.length - 1].src.replace(/^.*?\?/, "?");
  if (params[0] == "?")
    return params;
  return "";
}
var params = getParams();
var script = document.createElement("script");
script.src = "https://c328740.ssl.cf1.rackcdn.com/mathjax/latest/MathJax.js" + params;
document.head.appendChild(script);
