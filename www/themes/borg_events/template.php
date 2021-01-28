
<?php
/**
 * @file
 * Theme function overrides.
 */

/*******************************************************************************
 * Alter functions: modify renderable structures before used.
 ******************************************************************************/


/*******************************************************************************
 * Preprocess functions: prepare variables for templates.
 ******************************************************************************/


/******************************************************************************
 * Theme function: Replace existng theme functions with our own.
 ******************************************************************************/

/**
 * Overrides theme_menu_tree().
 */
function borg_events_menu_tree__user_menu($variables) {
  $variables['attributes']['class'][] = 'closed';

  global $user;

  $output  = '<nav class="borg-greeting">';
  $output .= '  <ul class="borg-user-menu">';
  $output .= '    <li class=top>';

  if ($user->uid) {
    $greeting = t('Hi @name!', array('@name'  => $user->name));
    $output .= '      <a href="#" id="greeting" class="greeting">' . $greeting . '</a>';
  }
  else {
    $output .= '      <a href="#" id="greeting" class="greeting">' . t('Welcome!') . '</a>';
  }

  $output .= '      <ul' . backdrop_attributes($variables['attributes']) . '>' . $variables['tree'] . '</ul>';
  $output .= '    </li>';
  $output .= '  </ul>';

  $output .= '  <a class="icon" title="Find us on GitHub" href="https://github.com/backdrop/backdrop"><i class="fa fa-github fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Follow os on Twitter" href="https://twitter.com/backdropcms"><i class="fa fa-twitter fa-2x" aria-hidden="true"></i></a>';
  $output .= '  <a class="icon" title="Subscribe to our Newsletter" href="https://backdropcms.org/newsletter"><i class="fa fa-envelope fa-2x" aria-hidden="true"></i></a>';

  $output .= '</nav>';

  return $output;
}
