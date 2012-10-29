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
 * Card Data Disable
 *
 * - Disabled cards have a card data status = 0.
 * - Disabled cards are removed from the remote gateway if the payment
 *   method supports deleting.
 *
 * @param $card_data
 *   The saved card data.
  * @param $card_data_original
 *   The original unchanged card data.
 */
function hook_commerce_cardonfile_data_disable($card_data, $card_data_original) {
  // perform custom operations reacting to the disabling of an existing
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

/**
 * Lets modules define the chargeable cards associated with an order
 *
 * The first chargeable card that passes the capability checks will be returned
 * as the order's chargeable_cardonfile entity property.  This property can
 * be used for recurring payments.
 *
 * @param $order
 *   The qualified order object. The order is ensured to have an owner user
 *   account.
 *
 * @return
 *   An array of card_ids for cards on file
 */
function hook_commerce_cardonfile_order_chargeable_cards($order) {
  $possible_cards = array();

  // Default card for payment instance
  if (!empty($order->data['payment_method'])) {
    $default_cards = commerce_cardonfile_data_load_user_default_cards($order->uid, $order->data['payment_method']);
    if (!empty($default_cards)) {
      $possible_cards = array_keys($default_cards);
    }
  }

  return $possible_cards;
}

/**
 * Lets modules alter the card on file form elements that get added to the
 * payment method form during checkout
 *
 * @param $pane_form
 *  The pane form as it would be presented to a payment module
 * @param $form
 *  The complete checkout page form
 */
function hook_commerce_cardonfile_checkout_pane_form_alter(&$pane_form, $form) {
  $pane_form['cardonfile_instance_default']['#description'] = t('Your default card will be used for automatic payments for all of your subscriptions.');
}
