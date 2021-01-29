(function ($) {

/**
 * Provide the summary information for the user_role plugin's vertical tab.
 */
Backdrop.behaviors.menuPositionUserRoleSettingsSummary = {
  attach: function (context) {
    $('fieldset#edit-user-role', context).backdropSetSummary(function (context) {
      var vals = [];
      $('input[type="checkbox"]:checked', context).each(function () {
        vals.push($.trim($(this).next('label').text()));
      });
      if (!vals.length) {
        vals.push(Backdrop.t('Any user role'));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
