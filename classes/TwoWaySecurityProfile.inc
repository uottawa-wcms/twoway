<?php

/**
  * Handles security for TwoWay communications.
  *
  * This is a base, non-secure implementation. You should develop your own to
  * use for your internal communication systems.
  */
class TwoWaySecurityProfile {

  /**
    * Called to secure the request upon creation.
    *
    * Recommended steps:
    * - Encrypt the password header
    * - Create a message ID message variable
    *
    * @param TwoWayRequest $request The request to secure
    */
  public function secureRequest(TwoWayRequest &$request) {}

  /**
    * Called to encrypt the outgoing message upon send.
    *
    * Recommended steps:
    * - Return an encrypted version of $compacted_message
    * - If you need to send a public key or IV of some kind, add a message variable
    *
    * @param string $compacted_message The compacted version of the message
    * @param TwoWayRequest $original The original request used to generate
    *   $compacted_message.
    * @return string The encrypted version of $compacted_message
    */
  public function encryptRequest($compacted_message, TwoWayRequest &$original) {
    return $compacted_message;
  }

  /**
   * Called to decrypt the incoming request.
   *
   * This is the reverse procedure from encryptRequest.
   *
   * @param string $compacted_message The encrypted message
   * @param array $vectors A list of unencrypted message headers.
   * @return string The decrypted message
   */
  public function decryptRequest($compacted_message, $vectors) {
    return $compacted_message;
  }

  /**
    * Generates an HMAC based on the incoming compacted message.
    *
    * This operates on the non-encrypted version of the message.
    *
    * @param string $compacted_message The compacted version of the message.
    * @return string An HMAC for the message.
    */
  public function generateHMAC($compacted_message) {
    return '';
  }

  /**
   * Examine the incoming message and see if it should be rejected.
   *
   * Ideally, this should examine the message headers and the information
   * stored in the request, and confirm that they are the same.
   *
   * @param TwoWayRequest $request The incoming request
   * @param array $vectors Message headers
   * @return boolean TRUE if the message should be rejected, otherwise FALSE.
   */
  public function invalidateRequest(TwoWayRequest &$request, $vectors) {
    return FALSE;
  }

  /**
   * This method reverses any security added in secureRequest.
   *
   * In this stage, you should decrypt the password field.
   *
   * You can save database-intensive operations for finalInvalidation.
   *
   * @param TwoWayRequest $request The incoming request
   */
  public function unsecureRequest(TwoWayRequest &$request) {}

  /**
   * A second chance to reject the message, after it's been unsecured.
   *
   * In this stage, you should validate the message ID and make sure it's
   * valid.   *
   *
   * @param TwoWayRequest $request The incoming request
   * @return boolean TRUE if the message should be rejected, otherwise FALSE.
   */
  public function finalInvalidation(TwoWayRequest &$request) {
    return FALSE;
  }

  /**
   * Allows the security profile to update the response headers before it's
   * sent.
   *
   * If you use a message ID, you can add it to the response object here.
   *
   * @param TwoWayResponse $response The response object.
   * @param TwoWayRequest $original_request The original request object.
   */
  public function prepareResponse(TwoWayResponse &$response, TwoWayRequest $original_request) {

  }

  /**
   * Encrypts the response message.
   *
   * This should return an encrypted version of $compacted_message. If you need
   * to store an IV or a public key, you can include it in the response headers.
   *
   * @param string $compacted_message The unencrypted message text.
   * @param TwoWayResponse $original The original response object
   * @return string The encrypted message text
   */
  public function encryptResponse($compacted_message, TwoWayResponse &$original) {
    return $compacted_message;
  }

  /**
   * Decrypts the response message.
   *
   * This should reverse the operations done in encryptResponse.
   *
   * @param type $compacted_message
   * @param type $vectors
   * @return type
   */
  public function decryptResponse($compacted_message, $vectors) {
    return $compacted_message;
  }

  /**
   * This allows you to replace which POST keys are used in the request.
   *
   * @param array $package The message package about to be sent
   * @return array The message package to actually send
   */
  public function obfuscateRequestPackage($package) {
    return $package;
  }

  /**
   * Returns the POST keys of the request to their original state.
   *
   * Reverses the operation done in obfuscateRequestPackage.
   *
   * @param array $package The incoming message package
   * @return array The deobfuscated version of the message package
   */
  public function deobfuscateRequestPackage($package) {
    return $package;
  }

  /**
   * Allows you to replace which POST keys are used in the response.
   *
   * @param array $package The message package about to be returned
   * @return array The message package to actually return
   */
  public function obfuscateResponsePackage($package) {
    return $package;
  }

  /**
   * Returns the POST keys of the response to their original state.
   *
   * Reverse the operation done in obfuscateResponsePackage.
   *
   * @param array $package The message package that was received
   * @return array The actual message package
   */
  public function deobfuscateResponsePackage($package) {
    return $package;
  }

  /**
   * Encrypts a remote password, for storage in the database.
   *
   * @param string $password Plain-text password
   * @param array $vectors Variables that you can use to encrypt the password
   * @return string The encrypted password
   */
  public function encryptServerPassword($password, $vectors) {
    return $password;
  }

  /**
   * Decrypts a remote password, from the database.
   *
   * @param string $password The encrypted password
   * @param array $vectors Variables that you can use to decrypt the password
   * @return string The plain-text password
   */
  public function decryptServerPassword($password, $vectors) {
    return $password;
  }

  /**
   * Checks the response from the remote server to ensure it's valid.
   *
   * Should validate that the correct fields are present and that they match
   * the headers.
   *
   * @param TwoWayRemoteResponse $response The response object
   * @param array $vectors The original message headers
   */
  public function validateResponse(TwoWayRemoteResponse $response, $vectors) {

  }



}