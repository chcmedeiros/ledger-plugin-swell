import { processTest, populateTransaction } from "./test.fixture";

const contractName = "TransparentUpgradeableProxy"; // <= Name of the smart contract

const testLabel = "eth_delete_pending_validators"; // <= Name of the test
const testDirSuffix = "eth_delete_pending_validators"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x46ddc39e780088b1b146aba8cbbe15dc321a1a1d"; // <= Address of the smart contract
const chainID = 1;

const inputData = "0x242eba0e00000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000002000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000000000000030a830a2cdf529ee03386076cc773570abd23bd6b10d8b1375e17a06dd0d2e759b6cb1168acd99117fcf26c15777dd7250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030a630a2cdf529ee03386076cc773570abd23bd6b10d8b1375e17a06dd0d2e759b6cb1168acd99117fcf26c15777dd725000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 12, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 7, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 7, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
