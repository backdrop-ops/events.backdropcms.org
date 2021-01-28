<?php

/**
 * @file
 * The class used for registration state entities
 */
class RegistrationState extends Entity {

  public $name, $label, $description, $default_state,
    $active, $held, $show_on_form, $weight;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'registration_state');
  }

  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return $this->name;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'registration_state';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return $this->label;
  }

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {
    return array();
  }

}
