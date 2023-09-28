#pragma once

// TODO(@zac-wiliamson #2341 rename to pedersen.hpp once we migrate to new hash standard)

#include "../pedersen_commitment/pedersen.hpp"
#include "barretenberg/ecc/curves/grumpkin/grumpkin.hpp"
#include <array>

namespace crypto {

/**
 * @brief Performs pedersen hashes!
 *
 * To hash to a size-n list of field elements `x`, we return the X-coordinate of:
 *
 *      Hash(x) = n.[h] + Commit(x)
 *
 * Where `g` is a list of generator points defined by `generator_data`
 * And `h` is a unique generator whose domain separator is the string `pedersen_hash_length`.
 *
 * The addition of `n.[h]` into the hash is to prevent length-extension attacks.
 * It also ensures that the hash output is never the point at infinity.
 *
 * It is neccessary that all generator points are linearly independent of one another,
 * so that finding collisions is equivalent to solving the discrete logarithm problem.
 * This is ensured via the generator derivation algorithm in `generator_data`
 */
template <typename Curve> class pedersen_hash_base {
  public:
    using AffineElement = typename Curve::AffineElement;
    using Element = typename Curve::Element;
    using Fq = typename Curve::BaseField;
    using Fr = typename Curve::ScalarField;
    using Group = typename Curve::Group;
    using GeneratorContext = typename crypto::GeneratorContext<Curve>;

    inline static constexpr AffineElement length_generator =
        Group::template derive_generators_secure<1>("pedersen_hash_length")[0];

    static Fq hash(const std::vector<Fq>& inputs, GeneratorContext context = {});
};

extern template class pedersen_hash_base<curve::Grumpkin>;

using pedersen_hash = pedersen_hash_base<curve::Grumpkin>;
} // namespace crypto
