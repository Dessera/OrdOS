/**
 * @brief Gets the current interrupt status
 *
 * @return true If interrupts are enabled
 * @return false If interrupts are disabled
 */
bool
intr_get_status(void);

/**
 * @brief Sets the current interrupt status
 *
 * @param status The new interrupt status
 * @return true If interrupts were enabled before setting
 * @return false If interrupts were disabled before setting
 */
bool
intr_set_status(bool status);