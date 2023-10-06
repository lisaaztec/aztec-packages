/* Autogenerated file, do not edit! */

/* eslint-disable */
import {
  AztecAddress,
  CompleteAddress,
  ContractBase,
  ContractFunctionInteraction,
  ContractMethod,
  DeployMethod,
  FieldLike,
  AztecAddressLike,
  EthAddressLike,
  Wallet,
} from '@aztec/aztec.js';
import { Fr, Point } from '@aztec/foundation/fields';
import { PXE, PublicKey } from '@aztec/types';
import { ContractAbi } from '@aztec/foundation/abi';
import BlankContractAbiJson from 'Blank.json' assert { type: 'json' };
export const BlankContractAbi = BlankContractAbiJson as ContractAbi;

/**
 * Type-safe interface for contract Blank;
 */
export class BlankContract extends ContractBase {
  private constructor(
    /** The deployed contract's complete address. */
    completeAddress: CompleteAddress,
    /** The wallet. */
    wallet: Wallet,
  ) {
    super(completeAddress, BlankContractAbi, wallet);
  }

  /**
   * Creates a contract instance.
   * @param address - The deployed contract's address.
   * @param wallet - The wallet to use when interacting with the contract.
   * @returns A promise that resolves to a new Contract instance.
   */
  public static async at(
    /** The deployed contract's address. */
    address: AztecAddress,
    /** The wallet. */
    wallet: Wallet,
  ) {
    const extendedContractData = await wallet.getExtendedContractData(address);
    if (extendedContractData === undefined) {
      throw new Error('Contract ' + address.toString() + ' is not deployed');
    }
    return new BlankContract(extendedContractData.getCompleteAddress(), wallet);
  }

  /**
   * Creates a tx to deploy a new instance of this contract.
   */
  public static deploy(pxe: PXE) {
    return new DeployMethod<BlankContract>(Point.ZERO, pxe, BlankContractAbi, Array.from(arguments).slice(1));
  }

  /**
   * Creates a tx to deploy a new instance of this contract using the specified public key to derive the address.
   */
  public static deployWithPublicKey(pxe: PXE, publicKey: PublicKey) {
    return new DeployMethod<BlankContract>(publicKey, pxe, BlankContractAbi, Array.from(arguments).slice(2));
  }

  /**
   * Returns this contract's ABI.
   */
  public static get abi(): ContractAbi {
    return BlankContractAbi;
  }

  /** Type-safe wrappers for the public methods exposed by the contract. */
  public methods!: {
    /** getPublicKey(address: field) */
    getPublicKey: ((address: FieldLike) => ContractFunctionInteraction) & Pick<ContractMethod, 'selector'>;
  };
}
