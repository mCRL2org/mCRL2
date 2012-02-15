OS = ['win', 'linux', 'mac'];

function getbuttons(div)
{
  var btns = [];
  if (div.hasClass('platform-specific'))
  {
    for (var i = 0; i < OS.length; ++i)
      if (div.hasClass(OS[i]))
        btns.push(OS[i]);
    btns = btns.concat(getbuttons(div.next()));
  }
  return btns;
}

function platformbar()
{
  var result = document.createElement('div');
  result.className = 'platform-buttons';
  btns = getbuttons($(this));
  for (var i = 0; i < btns.length; ++i)
  {
    var button = document.createElement('img');
    button.className = 'platform-button ' + btns[i];
    result.appendChild(button);
  }
  return result;
}

function showplatform(os)
{
  $('.platform-button').removeClass('platform-selected');
  $('.platform-button.' + os).addClass('platform-selected');
  $('.platform-specific').slideUp('slow');
  $('.platform-specific.' + os).slideDown('slow');
}

function clickplatform(e)
{
  for (var i = 0; i < 3; ++i)
    if ($(this).hasClass(OS[i]))
      showplatform(OS[i]);
}

/* Set up special behaviour for collapsing div's. */
$(document).ready(function () {
  $("div.collapse").children().addClass("collapsed");
  $("div.collapse").addClass("collapsed");
  $("div.collapse p.admonition-title").bind("click", function (e) {
    $(this).parent().children().toggleClass("collapsed");
    $(this).parent().toggleClass("collapsed");
  });
  $(':not(.platform-specific) + .platform-specific').before(platformbar);
  $('.platform-specific').hide();
  $('.platform-button').bind('click', clickplatform);
  showplatform('win');
});
