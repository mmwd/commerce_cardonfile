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
