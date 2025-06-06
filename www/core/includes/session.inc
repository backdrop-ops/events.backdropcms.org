<?php
/**
 * @file
 * User session handling functions.
 *
 * The user-level session storage handlers:
 * - _backdrop_session_open()
 * - _backdrop_session_close()
 * - _backdrop_session_read()
 * - _backdrop_session_write()
 * - _backdrop_session_destroy()
 * - _backdrop_session_garbage_collection()
 * are assigned by session_set_save_handler() in bootstrap.inc and are called
 * automatically by PHP. These functions should not be called directly. Session
 * data should instead be accessed via the $_SESSION superglobal.
 *
 * @see https://www.php.net/manual/en/reserved.variables.session.php
 */

/**
 * Provides the default session handler for Backdrop.
 *
 * PHP 8.4 and higher require session_set_save_handler() use a class instance
 * rather than separate function callbacks. The separate callbacks are retained
 * in this file for backwards-compatibility.
 */
class BackdropSessionHandler implements SessionHandlerInterface {
  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function open($path = '', $name = '') {
    return _backdrop_session_open();
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function close() {
    return _backdrop_session_close();
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function read($id) {
    return _backdrop_session_read($id);
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function write($id, $data) {
    return _backdrop_session_write($id, $data);
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function destroy($id) {
    return _backdrop_session_destroy($id);
  }

  /**
   * {@inheritdoc}
   */
  #[\ReturnTypeWillChange]
  public function gc($max_lifetime) {
    return _backdrop_session_garbage_collection($max_lifetime);
  }
}

/**
 * Session handler assigned by session_set_save_handler().
 *
 * This function is used to handle any initialization, such as file paths or
 * database connections, that is needed before accessing session data. Backdrop
 * does not need to initialize anything in this function.
 *
 * This function should not be called directly.
 *
 * @return
 *   This function will always return TRUE.
 */
function _backdrop_session_open() {
  return TRUE;
}

/**
 * Session handler assigned by session_set_save_handler().
 *
 * This function is used to close the current session. Because Backdrop stores
 * session data in the database immediately on write, this function does
 * not need to do anything.
 *
 * This function should not be called directly.
 *
 * @return
 *   This function will always return TRUE.
 */
function _backdrop_session_close() {
  return TRUE;
}

/**
 * Reads an entire session from the database (internal use only).
 *
 * Also initializes the $user object for the user associated with the session.
 * This function is registered with session_set_save_handler() to support
 * database-backed sessions. It is called on every page load when PHP sets
 * up the $_SESSION superglobal.
 *
 * This function is an internal function and must not be called directly.
 * Doing so may result in logging out the current user, corrupting session data
 * or other unexpected behavior. Session data must always be accessed via the
 * $_SESSION superglobal.
 *
 * @see https://www.php.net/manual/en/reserved.variables.session.php
 *
 * @param $sid
 *   The session ID of the session to retrieve.
 *
 * @return
 *   The user's session, or an empty string if no session exists.
 */
function _backdrop_session_read($sid) {
  global $user, $is_https;

  // Write and Close handlers are called after destructing objects
  // since PHP 5.0.5.
  // Thus destructors can use sessions but session handler can't use objects.
  // So we are moving session closure before destructing objects.
  backdrop_register_shutdown_function('session_write_close');

  // Handle the case of first time visitors and clients that don't store
  // cookies (eg. web crawlers).
  $insecure_session_name = substr(session_name(), 1);
  if (empty($sid) || (!isset($_COOKIE[session_name()]) && !isset($_COOKIE[$insecure_session_name]))) {
    $user = backdrop_anonymous_user();
    return '';
  }

  // Otherwise, if the session is still active, we have a record of the
  // client's session in the database. If it's HTTPS then we are either have
  // a HTTPS session or we are about to log in so we check the sessions table
  // for an anonymous session with the non-HTTPS-only cookie.
  if ($is_https) {
    $user = db_query("SELECT u.*, s.* FROM {users} u INNER JOIN {sessions} s ON u.uid = s.uid WHERE s.ssid = :ssid", array(':ssid' => $sid))->fetchObject();
    if (!$user) {
      if (isset($_COOKIE[$insecure_session_name])) {
        $user = db_query("SELECT u.*, s.* FROM {users} u INNER JOIN {sessions} s ON u.uid = s.uid WHERE s.sid = :sid AND s.uid = 0", array(
        ':sid' => $_COOKIE[$insecure_session_name]))
        ->fetchObject();
      }
    }
  }
  else {
    $user = db_query("SELECT u.*, s.* FROM {users} u INNER JOIN {sessions} s ON u.uid = s.uid WHERE s.sid = :sid", array(':sid' => $sid))->fetchObject();
  }

  // We found the client's session record and they are an authenticated,
  // active user.
  if ($user && $user->uid > 0 && $user->status == 1) {
    // This is done to unserialize the data member of $user.
    $user->data = unserialize((string) $user->data);

    // Add roles element to $user.
    $user->roles = db_query("SELECT role FROM {users_roles} WHERE uid = :uid", array(':uid' => $user->uid))->fetchCol();
    array_unshift($user->roles, BACKDROP_AUTHENTICATED_ROLE);
  }
  elseif ($user) {
    // The user is anonymous or blocked. Only preserve two fields from the
    // {sessions} table.
    $account = backdrop_anonymous_user();
    $account->session = $user->session;
    $account->timestamp = $user->timestamp;
    $user = $account;
  }
  else {
    // The session has expired.
    $user = backdrop_anonymous_user();
    $user->session = '';
  }

  // Store the session that was read for comparison in _backdrop_session_write().
  $last_read = &backdrop_static('backdrop_session_last_read');
  $last_read = array(
    'sid' => $sid,
    'value' => $user->session,
  );

  return $user->session;
}

/**
 * Writes an entire session to the database (internal use only).
 *
 * This function is registered with session_set_save_handler() to support
 * database-backed sessions.
 *
 * This function is an internal function and must not be called directly.
 * Doing so may result in corrupted session data or other unexpected behavior.
 * Session data must always be accessed via the $_SESSION superglobal.
 *
 * @see https://www.php.net/manual/en/reserved.variables.session.php
 *
 * @param $sid
 *   The session ID of the session to write to.
 * @param $value
 *   Session data to write as a serialized string.
 *
 * @return
 *   Always returns TRUE.
 */
function _backdrop_session_write($sid, $value) {
  global $user, $is_https;

  // The exception handler is not active at this point, so we need to do it
  // manually.
  try {
    if (!backdrop_save_session()) {
      // We don't have anything to do if we are not allowed to save the session.
      return TRUE;
    }

    // Check whether $_SESSION has been changed in this request.
    $last_read = &backdrop_static('backdrop_session_last_read');
    $is_changed = !isset($last_read) || $last_read['sid'] != $sid || $last_read['value'] !== $value;

    // For performance reasons, do not update the sessions table, unless
    // $_SESSION has changed or more than 180 has passed since the last update.
    if ($is_changed || !isset($user->timestamp) || REQUEST_TIME - $user->timestamp > settings_get('session_write_interval', 180)) {
      // Either ssid or sid or both will be added from $key below.
      $fields = array(
        'uid' => $user->uid,
        'hostname' => ip_address(),
        'session' => $value,
        'timestamp' => REQUEST_TIME,
      );

      // Use the session ID as 'sid' and an empty string as 'ssid' by default.
      // _backdrop_session_read() does not allow empty strings so that's a safe
      // default.
      $key = array('sid' => $sid, 'ssid' => '');
      // On HTTPS connections, use the session ID as both 'sid' and 'ssid'.
      if ($is_https) {
        $key['ssid'] = $sid;
        // The "secure pages" setting allows a site to simultaneously use both
        // secure and insecure session cookies. If enabled and both cookies are
        // presented then use both keys.
        if (settings_get('https', FALSE)) {
          $insecure_session_name = substr(session_name(), 1);
          if (isset($_COOKIE[$insecure_session_name])) {
            $key['sid'] = $_COOKIE[$insecure_session_name];
          }
        }
      }
      elseif (settings_get('https', FALSE)) {
        unset($key['ssid']);
      }

      db_merge('sessions')
        ->key($key)
        ->fields($fields)
        ->execute();
    }

    // Likewise, do not update access time more than once per 180 seconds.
    if ($user->uid && REQUEST_TIME - $user->access > settings_get('session_write_interval', 180)) {
      db_update('users')
        ->fields(array(
          'access' => REQUEST_TIME
        ))
        ->condition('uid', $user->uid)
        ->execute();
    }

    return TRUE;
  }
  catch (Exception $exception) {
    require_once BACKDROP_ROOT . '/core/includes/errors.inc';
    // If we are displaying errors, then do so with no possibility of a further
    // uncaught exception being thrown.
    if (error_displayable()) {
      print '<h1>Uncaught exception thrown in session handler.</h1>';
      print '<p>' . _backdrop_render_exception_safe($exception) . '</p><hr />';
    }
    return FALSE;
  }
}

/**
 * Initializes the session handler, starting a session if needed.
 */
function backdrop_session_initialize() {
  global $user, $is_https;

  // Calling session_set_save_handler() multiple times will cause errors on
  // PHP 7.2+.
  if (session_status() === PHP_SESSION_NONE) {
    $session_handler = new BackdropSessionHandler();
    session_set_save_handler($session_handler);
  }

  // We use !empty() in the following check to ensure that blank session IDs
  // are not valid.
  if (!empty($_COOKIE[session_name()]) || ($is_https && settings_get('https', FALSE) && !empty($_COOKIE[substr(session_name(), 1)]))) {
    // If a session cookie exists, initialize the session. Otherwise the
    // session is only started on demand in backdrop_session_commit(), making
    // anonymous users not use a session cookie unless something is stored in
    // $_SESSION. This allows HTTP proxies to cache anonymous page views.
    backdrop_session_start();
    if (!empty($user->uid) || !empty($_SESSION)) {
      backdrop_page_is_cacheable(FALSE);
    }
  }
  else {
    // Set a session identifier for this request. This is necessary because
    // we lazily start sessions at the end of this request, and some
    // processes (like backdrop_get_token()) needs to know the future
    // session ID in advance.
    $GLOBALS['lazy_session'] = TRUE;
    $user = backdrop_anonymous_user();
    // Less random sessions (which are much faster to generate) are used for
    // anonymous users than are generated in backdrop_session_regenerate() when
    // a user becomes authenticated.
    session_id(backdrop_random_key());
    if ($is_https && settings_get('https', FALSE)) {
      $insecure_session_name = substr(session_name(), 1);
      $session_id = backdrop_random_key();
      $_COOKIE[$insecure_session_name] = $session_id;
    }
  }
  date_default_timezone_set(backdrop_get_user_timezone());
}

/**
 * Starts a session forcefully, preserving already set session data.
 *
 * @ingroup php_wrappers
 */
function backdrop_session_start() {
  // Command line clients do not support cookies nor sessions.
  if (!backdrop_session_started() && !backdrop_is_cli()) {
    // Save current session data before starting it, as PHP will destroy it.
    $session_data = isset($_SESSION) ? $_SESSION : NULL;

    session_start();
    backdrop_session_started(TRUE);

    // Restore session data.
    if (!empty($session_data)) {
      $_SESSION += $session_data;
    }
  }
}

/**
 * Commits the current session, if necessary.
 *
 * If an anonymous user already have an empty session, destroy it.
 */
function backdrop_session_commit() {
  global $user, $is_https;

  if (!backdrop_save_session()) {
    // We don't have anything to do if we are not allowed to save the session.
    return TRUE;
  }

  if (empty($user->uid) && empty($_SESSION)) {
    // There is no session data to store, destroy the session if it was
    // previously started.
    if (backdrop_session_started()) {
      session_destroy();
    }
  }
  else {
    // There is session data to store. Start the session if it is not already
    // started.
    if (!backdrop_session_started()) {
      backdrop_session_start();
      if ($is_https && settings_get('https', FALSE)) {
        $insecure_session_name = substr(session_name(), 1);
        $params = session_get_cookie_params();
        $expire = $params['lifetime'] ? REQUEST_TIME + $params['lifetime'] : 0;
        setcookie($insecure_session_name, $_COOKIE[$insecure_session_name], $expire, $params['path'], $params['domain'], FALSE, $params['httponly']);
      }
    }
    // Write the session data.
    session_write_close();
  }
  return TRUE;
}

/**
 * Returns whether a session has been started.
 */
function backdrop_session_started($set = NULL) {
  static $session_started = FALSE;
  if (isset($set)) {
    $session_started = $set;
  }
  return $session_started && session_id();
}

/**
 * Called when an anonymous user becomes authenticated or vice-versa.
 *
 * @ingroup php_wrappers
 */
function backdrop_session_regenerate() {
  global $user, $is_https;
  // Nothing to do if we are not allowed to change the session.
  if (!backdrop_save_session()) {
    return;
  }

  if ($is_https && settings_get('https', FALSE)) {
    $insecure_session_name = substr(session_name(), 1);
    if (!isset($GLOBALS['lazy_session']) && isset($_COOKIE[$insecure_session_name])) {
      $old_insecure_session_id = $_COOKIE[$insecure_session_name];
    }
    $params = session_get_cookie_params();
    $session_id = backdrop_random_key();
    // If a session cookie lifetime is set, the session will expire
    // $params['lifetime'] seconds from the current request. If it is not set,
    // it will expire when the browser is closed.
    $expire = $params['lifetime'] ? REQUEST_TIME + $params['lifetime'] : 0;
    setcookie($insecure_session_name, $session_id, $expire, $params['path'], $params['domain'], FALSE, $params['httponly']);
    $_COOKIE[$insecure_session_name] = $session_id;
  }

  if (backdrop_session_started()) {
    $old_session_id = session_id();
    _backdrop_session_regenerate_existing();
  }
  else {
    session_id(backdrop_random_key());
  }

  if (isset($old_session_id)) {
    $params = session_get_cookie_params();
    $expire = $params['lifetime'] ? REQUEST_TIME + $params['lifetime'] : 0;
    setcookie(session_name(), session_id(), $expire, $params['path'], $params['domain'], $params['secure'], $params['httponly']);
    $fields = array('sid' => session_id());
    if ($is_https) {
      $fields['ssid'] = session_id();
      // If the "secure pages" setting is enabled, use the newly-created
      // insecure session identifier as the regenerated sid.
      if (settings_get('https', FALSE)) {
        $fields['sid'] = $session_id;
      }
    }
    db_update('sessions')
      ->fields($fields)
      ->condition($is_https ? 'ssid' : 'sid', $old_session_id)
      ->execute();
  }
  elseif (isset($old_insecure_session_id)) {
    // If logging in to the secure site, and there was no active session on the
    // secure site but a session was active on the insecure site, update the
    // insecure session with the new session identifiers.
    db_update('sessions')
      ->fields(array('sid' => $session_id, 'ssid' => session_id()))
      ->condition('sid', $old_insecure_session_id)
      ->execute();
  }
  else {
    // Start the session when it doesn't exist yet.
    // Preserve the logged in user, as it will be reset to anonymous
    // by _backdrop_session_read.
    $account = $user;
    backdrop_session_start();
    $user = $account;
  }
  date_default_timezone_set(backdrop_get_user_timezone());
}

/**
 * Regenerates an existing session.
 */
function _backdrop_session_regenerate_existing() {
  global $user;
  // Preserve existing settings for the saving of sessions.
  $original_save_session_status = backdrop_save_session();
  // Turn off saving of sessions.
  backdrop_save_session(FALSE);
  session_write_close();
  backdrop_session_started(FALSE);
  // Preserve the user object, as starting a new session will reset it.
  $original_user = $user;
  session_id(backdrop_random_key());
  backdrop_session_start();
  $user = $original_user;
  // Restore the original settings for the saving of sessions.
  backdrop_save_session($original_save_session_status);
}

/**
 * Session handler assigned by session_set_save_handler().
 *
 * Cleans up a specific session.
 *
 * @param $sid
 *   Session ID.
 *
 * @return TRUE
 *   The session destroy handler must always return TRUE.
 */
function _backdrop_session_destroy($sid) {
  global $user, $is_https;

  // Nothing to do if we are not allowed to change the session.
  if (!backdrop_save_session()) {
    return TRUE;
  }

  // Delete session data.
  db_delete('sessions')
    ->condition($is_https ? 'ssid' : 'sid', $sid)
    ->execute();

  // Reset $_SESSION and $user to prevent a new session from being started
  // in backdrop_session_commit().
  $_SESSION = array();
  $user = backdrop_anonymous_user();

  // Unset the session cookies.
  _backdrop_session_delete_cookie(session_name());
  if ($is_https) {
    _backdrop_session_delete_cookie(substr(session_name(), 1), FALSE);
  }
  elseif (settings_get('https', FALSE)) {
    _backdrop_session_delete_cookie('S' . session_name(), TRUE);
  }
  return TRUE;
}

/**
 * Deletes the session cookie.
 *
 * @param $name
 *   Name of session cookie to delete.
 * @param boolean $secure
 *   Force the secure value of the cookie.
 */
function _backdrop_session_delete_cookie($name, $secure = NULL) {
  global $is_https;
  if (isset($_COOKIE[$name]) || (!$is_https && $secure === TRUE)) {
    $params = session_get_cookie_params();
    if ($secure !== NULL) {
      $params['secure'] = $secure;
    }
    setcookie($name, '', REQUEST_TIME - 3600, $params['path'], $params['domain'], $params['secure'], $params['httponly']);
    unset($_COOKIE[$name]);
  }
}

/**
 * Ends a specific user's session(s).
 *
 * @param $uid
 *   User ID.
 */
function backdrop_session_destroy_uid($uid) {
  // Nothing to do if we are not allowed to change the session.
  if (!backdrop_save_session()) {
    return;
  }

  db_delete('sessions')
    ->condition('uid', $uid)
    ->execute();
}

/**
 * Session handler assigned by session_set_save_handler().
 *
 * Cleans up stalled sessions.
 *
 * @param $lifetime
 *   The value of session.gc_maxlifetime, passed by PHP.
 *   Sessions not updated for more than $lifetime seconds will be removed.
 *
 * @return TRUE
 *   The garbage collection handler must always return TRUE.
 */
function _backdrop_session_garbage_collection($lifetime) {
  // Be sure to adjust 'php_value session.gc_maxlifetime' to a large enough
  // value. For example, if you want user sessions to stay in your database
  // for three weeks before deleting them, you need to set gc_maxlifetime
  // to '1814400'. At that value, only after a user doesn't log in after
  // three weeks (1814400 seconds) will his/her session be removed.
  db_delete('sessions')
    ->condition('timestamp', REQUEST_TIME - $lifetime, '<')
    ->execute();
  return TRUE;
}

/**
 * Determines whether to save session data of the current request.
 *
 * This function allows the caller to temporarily disable writing of
 * session data, should the request end while performing potentially
 * dangerous operations, such as manipulating the global $user object.
 * See http://drupal.org/node/218104 for usage.
 *
 * @param $status
 *   Disables writing of session data when FALSE, (re-)enables
 *   writing when TRUE.
 *
 * @return
 *   FALSE if writing session data has been disabled. Otherwise, TRUE.
 */
function backdrop_save_session($status = NULL) {
  // PHP session ID, session, and cookie handling happens in the global scope.
  // This value has to persist across calls to backdrop_static_reset(), since a
  // potentially wrong or disallowed session would be written otherwise.
  static $save_session = TRUE;
  if (isset($status)) {
    $save_session = $status;
  }
  return $save_session;
}
