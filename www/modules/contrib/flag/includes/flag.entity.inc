<?php

/**
 * @file
 * Provides supporting code for the entity/fields system.
 *
 * Note: We're making the <em>flaggings</em> fieldable, not the <em>flags</em>.
 * (In the same way that Backdrop makes <em>nodes</em> fieldable, not <em>node
 * types</em>).
 */

/**
 * Defines the flagging entity class.
 */
class Flagging extends Entity {

  /**
   * The flagging ID.
   *
   * @var integer
   */
  public $flagging_id;

  /**
   * The flag name.
   *
   * @var string
   */
  public $flag_name;

  /**
   * The entity type which is being flagged.
   *
   * @var string
   */
  public $entity_type;


  /**
   * The entity ID.
   *
   * @var integer
   */
  public $entity_id;

  /**
   * The flagging user ID.
   *
   * @var string
   */
  public $uid;

  /**
   * The user session ID.
   *
   * @var integer
   */
  public $sid;


  /**
   * Implements EntityInterface::id().
   */
  public function id() {
    return $this->flagging_id;
  }

  /**
   * Implements EntityInterface::entityType().
   */
  public function entityType() {
    return 'flagging';
  }

  /**
   * Implements EntityInterface::label().
   */
  public function label() {
    return $this->flagging_id;
  }

  /**
   * Implements EntityInterface::uri().
   */
  public function uri() {
    return array(
      'path' => '',
    );
  }
}
