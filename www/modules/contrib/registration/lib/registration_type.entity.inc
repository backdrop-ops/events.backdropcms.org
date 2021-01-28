<?php

/**
 * @file
 * The class used for model type entities
 */
class RegistrationType extends Entity {

  public $name;
  public $label;
  public $locked;
  public $default_state;
  public $data;

  public function __construct(array $values = array()) {
    parent::__construct($values, 'registration_type');
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
    return 'registration_type';
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
