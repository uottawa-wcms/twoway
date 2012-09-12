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
    * Called to encrypt the outgoing message upon send
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

  public function decryptRequest($compacted_message, $base_message) {
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
   *
   * @param TwoWayRequest $request
   * @return boolean
   */
  public function invalidateRequest(TwoWayRequest &$request, $envelope_variables) {
    return FALSE;
  }

  public function unsecureRequest(TwoWayRequest &$request) {}

  public function finalInvalidation(TwoWayRequest &$request) {
    return FALSE;
  }

  public function prepareResponse(TwoWayResponse &$response, TwoWayRequest $original_request) {

  }

  public function encryptResponse($compacted_message, TwoWayResponse &$original) {
    return $compacted_message;
  }

  public function decryptResponse($compacted_message, $vectors) {
    return $compacted_message;
  }

  public function obfuscateRequestPackage($package) {
    return $package;
  }

  public function deobfuscateRequestPackage($package) {
    return $package;
  }

  public function obfuscateResponsePackage($package) {
    return $package;
  }

  public function deobfuscateResponsePackage($package) {
    return $package;
  }

  public function decryptServerPassword($password, $vectors) {
    return $password;
  }

  public function encryptServerPassword($password, $vectors) {
    return $password;
  }

  public function validateResponse(TwoWayRemoteResponse $response, $vectors) {
    
  }



}