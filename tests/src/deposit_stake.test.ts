import { processTest, populateTransaction } from "./test.fixture";

const contractName = "TransparentUpgradeableProxy"; // <= Name of the smart contract

const testLabel = "eth_deposit_stake"; // <= Name of the test
const testDirSuffix = "eth_deposit_stake"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0xf951e335afb289353dc249e82926178eac7ded78"; // <= Address of the smart contract
const chainID = 1;

const inputData = "0xd0e30db0";
// Create serializedTx and remove the "0x" prefix
const value = "2";
const serializedTx = populateTransaction(
  contractAddr,
  inputData,
  chainID,
  value
);
const devices = [
  {
    name: "nanos",
    label: "Nano S",
    steps: 4, // <= Define the number of steps for this test case and this device
  },
  {
    name: "nanox",
    label: "Nano X",
    steps: 4, // <= Define the number of steps for this test case and this device
  },
  {
    name: "nanosp",
    label: "Nano S+",
    steps: 4, // <= Define the number of steps for this test case and this device
  },
];

devices.forEach((device) =>
  processTest(
    device,
    contractName,
    testLabel,
    testDirSuffix,
    "",
    signedPlugin,
    serializedTx,
    testNetwork
  )
);
