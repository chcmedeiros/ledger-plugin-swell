import { processTest, populateTransaction } from "./test.fixture";

const contractName = "TransparentUpgradeableProxy"; // <= Name of the smart contract

const testLabel = "eth_delete_active_validators"; // <= Name of the test
const testDirSuffix = "eth_delete_active_validators"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x46ddc39e780088b1b146aba8cbbe15dc321a1a1d"; // <= Address of the smart contract
const chainID = 1;

const inputData = "0xeda74e7100000000000000000000000000000000000000000000000000000000000000200000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000001200000000000000000000000000000000000000000000000000000000000000030a830a2cdf529ee03386076cc773570abd23bd6b10d8b1375e17a06dd0d2e759b6cb1168acd99117fcf26c15777dd7250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030a630a2cdf529ee03386076cc773570abd23bd6b10d8b1375e17a06dd0d2e759b6cb1168acd99117fcf26c15777dd7250000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030a930a2cdf529ee03386076cc773570abd23bd6b10d8b1375e17a06dd0d2e759b6cb1168acd99117fcf26c15777dd725000000000000000000000000000000000";
// Create serializedTx and remove the "0x" prefix
const value = "3.1";
const serializedTx = populateTransaction(contractAddr, inputData, chainID, value);
const devices = [
   {
     name: "nanos",
     label: "Nano S",
     steps: 16, // <= Define the number of steps for this test case and this device
   },
  {
    name: "nanox",
    label: "Nano X",
    steps: 9, // <= Define the number of steps for this test case and this device
  },
  {
     name: "nanosp",
     label: "Nano S+",
     steps: 9, // <= Define the number of steps for this test case and this device
   },
];

devices.forEach((device) =>
  processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
