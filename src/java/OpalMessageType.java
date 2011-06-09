/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 1.3.36
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */

package org.opalvoip;

public final class OpalMessageType {
  public final static OpalMessageType OpalIndCommandError = new OpalMessageType("OpalIndCommandError");
  public final static OpalMessageType OpalCmdSetGeneralParameters = new OpalMessageType("OpalCmdSetGeneralParameters");
  public final static OpalMessageType OpalCmdSetProtocolParameters = new OpalMessageType("OpalCmdSetProtocolParameters");
  public final static OpalMessageType OpalCmdRegistration = new OpalMessageType("OpalCmdRegistration");
  public final static OpalMessageType OpalIndRegistration = new OpalMessageType("OpalIndRegistration");
  public final static OpalMessageType OpalCmdSetUpCall = new OpalMessageType("OpalCmdSetUpCall");
  public final static OpalMessageType OpalIndIncomingCall = new OpalMessageType("OpalIndIncomingCall");
  public final static OpalMessageType OpalCmdAnswerCall = new OpalMessageType("OpalCmdAnswerCall");
  public final static OpalMessageType OpalCmdClearCall = new OpalMessageType("OpalCmdClearCall");
  public final static OpalMessageType OpalIndAlerting = new OpalMessageType("OpalIndAlerting");
  public final static OpalMessageType OpalIndEstablished = new OpalMessageType("OpalIndEstablished");
  public final static OpalMessageType OpalIndUserInput = new OpalMessageType("OpalIndUserInput");
  public final static OpalMessageType OpalIndCallCleared = new OpalMessageType("OpalIndCallCleared");
  public final static OpalMessageType OpalCmdHoldCall = new OpalMessageType("OpalCmdHoldCall");
  public final static OpalMessageType OpalCmdRetrieveCall = new OpalMessageType("OpalCmdRetrieveCall");
  public final static OpalMessageType OpalCmdTransferCall = new OpalMessageType("OpalCmdTransferCall");
  public final static OpalMessageType OpalCmdUserInput = new OpalMessageType("OpalCmdUserInput");
  public final static OpalMessageType OpalIndMessageWaiting = new OpalMessageType("OpalIndMessageWaiting");
  public final static OpalMessageType OpalIndMediaStream = new OpalMessageType("OpalIndMediaStream");
  public final static OpalMessageType OpalCmdMediaStream = new OpalMessageType("OpalCmdMediaStream");
  public final static OpalMessageType OpalCmdSetUserData = new OpalMessageType("OpalCmdSetUserData");
  public final static OpalMessageType OpalIndLineAppearance = new OpalMessageType("OpalIndLineAppearance");
  public final static OpalMessageType OpalCmdStartRecording = new OpalMessageType("OpalCmdStartRecording");
  public final static OpalMessageType OpalCmdStopRecording = new OpalMessageType("OpalCmdStopRecording");
  public final static OpalMessageType OpalIndProceeding = new OpalMessageType("OpalIndProceeding");
  public final static OpalMessageType OpalCmdAlerting = new OpalMessageType("OpalCmdAlerting");
  public final static OpalMessageType OpalMessageTypeCount = new OpalMessageType("OpalMessageTypeCount");

  public final int swigValue() {
    return swigValue;
  }

  public String toString() {
    return swigName;
  }

  public static OpalMessageType swigToEnum(int swigValue) {
    if (swigValue < swigValues.length && swigValue >= 0 && swigValues[swigValue].swigValue == swigValue)
      return swigValues[swigValue];
    for (int i = 0; i < swigValues.length; i++)
      if (swigValues[i].swigValue == swigValue)
        return swigValues[i];
    throw new IllegalArgumentException("No enum " + OpalMessageType.class + " with value " + swigValue);
  }

  private OpalMessageType(String swigName) {
    this.swigName = swigName;
    this.swigValue = swigNext++;
  }

  private OpalMessageType(String swigName, int swigValue) {
    this.swigName = swigName;
    this.swigValue = swigValue;
    swigNext = swigValue+1;
  }

  private OpalMessageType(String swigName, OpalMessageType swigEnum) {
    this.swigName = swigName;
    this.swigValue = swigEnum.swigValue;
    swigNext = this.swigValue+1;
  }

  private static OpalMessageType[] swigValues = { OpalIndCommandError, OpalCmdSetGeneralParameters, OpalCmdSetProtocolParameters, OpalCmdRegistration, OpalIndRegistration, OpalCmdSetUpCall, OpalIndIncomingCall, OpalCmdAnswerCall, OpalCmdClearCall, OpalIndAlerting, OpalIndEstablished, OpalIndUserInput, OpalIndCallCleared, OpalCmdHoldCall, OpalCmdRetrieveCall, OpalCmdTransferCall, OpalCmdUserInput, OpalIndMessageWaiting, OpalIndMediaStream, OpalCmdMediaStream, OpalCmdSetUserData, OpalIndLineAppearance, OpalCmdStartRecording, OpalCmdStopRecording, OpalIndProceeding, OpalCmdAlerting, OpalMessageTypeCount };
  private static int swigNext = 0;
  private final int swigValue;
  private final String swigName;
}

