<?php

/**
 * @file
 * UI Controller for Registration states.
 */
class RegistrationStatesUIController extends EntityDefaultUIController {

  public function overviewForm($form, &$form_state) {
    return backdrop_get_form('registration_states_overview_form');
  }

}
