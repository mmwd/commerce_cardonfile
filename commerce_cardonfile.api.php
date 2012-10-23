<?php

/**
 * @file
 * Hooks provided by the Commerce Card On File module.
 */

/**
 * Card Data Insert
 *
 * @param $card_data
 *   The inserted card data.
 */
function hook_commerce_cardonfile_data_insert($card_data) {
  // perform custom operations reacting to the creation of a new card data
}

/**
 * Card Data Update
 *
 * @param $card_data
 *   The saved card data.
  * @param $card_data_original
 *   The original unchanged card data.
 */
function hook_commerce_cardonfile_data_update($card_data, $card_data_original) {
  // perform custom operations reacting to the update of an existing
  // card data
}

/**
 * Card Data Delete
 *
 * @param $card_data
 *   The deleted card data.
 */
function hook_commerce_cardonfile_data_delete($card_data) {
  // perform custom operations reacting to the deletion of an existing
  // card data
}

/**
 * Lets modules prevent the deletion of a particular card data.
 *
 * Before a card can be deleted or disabled, other modules are given the chance
 * to say whether or not the action should be allowed. Modules implementing this
 * hook can check for reference data or any other reason to prevent a product
 * from being deleted and return FALSE to prevent the action.
 *
 * This is an API level hook, so implementations should not display any messages
 * to the user (although logging to the watchdog is fine).
 *
 * @param $card_data
 *   The card data to be deleted.
 *
 * @return
 *   TRUE or FALSE indicating whether or not the given card can be deleted.
 */
function hook_commerce_cardonfile_data_can_delete($card_data) {
  if (!empty($card_data['card_id'])) {
    // Use EntityFieldQuery to look for orders referencing this card and do
    // not allow the delete to occur if one exists.
    $query = new EntityFieldQuery();

    $query
      ->entityCondition('entity_type', 'commerce_order', '=')
      ->entityCondition('bundle', 'commerce_order', '=')
      ->fieldCondition('commerce_cardonfile', 'card_id', $card_data['card_id'], '=')
      ->count();

    return $query->execute() > 0 ? FALSE : TRUE;
  }
}
