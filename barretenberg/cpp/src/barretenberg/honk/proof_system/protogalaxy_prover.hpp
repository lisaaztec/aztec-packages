#pragma once
#include "barretenberg/honk/flavor/goblin_ultra.hpp"
#include "barretenberg/honk/flavor/ultra.hpp"
#include "barretenberg/honk/flavor/ultra_grumpkin.hpp"
#include "barretenberg/honk/instance/instances.hpp"
#include "barretenberg/honk/proof_system/folding_result.hpp"
#include "barretenberg/proof_system/flavor/flavor.hpp"
#include "barretenberg/proof_system/relations/utils.hpp"
namespace proof_system::honk {
template <class ProverInstances> class ProtoGalaxyProver_ {
  public:
    using Flavor = typename ProverInstances::Flavor;
    using Instance = typename ProverInstances::Instance;
    using Utils = barretenberg::RelationUtils<Flavor>;
    using RowEvaluations = typename Flavor::ProverPolynomialsEvaluations;
    using RelationEvaluations = typename Flavor::RelationValues;
    using FF = typename Flavor::FF;
    using ProverPolynomials = typename Flavor::ProverPolynomials;

    ProverInstances instances;
    ProverTranscript<FF> transcript;

    ProtoGalaxyProver_(ProverInstances insts)
        : instances(insts){};
    ~ProtoGalaxyProver_() = default;

    void prepare_for_folding();

    /**
     * @brief For a new round challenge δ at each iteration of the ProtoGalaxy protocol, compute the vector
     * [δ, δ^2,..., δ^t] where t = logn and n is the size of the instance.
     */
    static std::vector<FF> compute_round_challenge_pows(size_t log_instance_size, FF round_challenge)
    {
        std::vector<FF> pows(log_instance_size);
        pows[0] = round_challenge;
        for (size_t i = 1; i < log_instance_size; i++) {
            pows[i] = pows[i - 1].sqr();
        }
        return pows;
    }

    static RowEvaluations get_execution_row(std::shared_ptr<Instance> accumulator, size_t row)
    {
        RowEvaluations row_evals;
        size_t idx = 0;
        for (auto& poly : accumulator->prover_polynomials) {
            row_evals[idx] = poly[row];
            idx++;
        }
        return row_evals;
    }

    std::shared_ptr<Instance> get_accumulator() { return instances[0]; }

    /**
     * @brief Given the evaluations of all the prover polynomials at row i and the parameters that help establish each
     * subrelation is independently valid, compute the value of the full Honk relation for that specific row (this is
     * f_i(ω) in the paper)
     */
    static FF compute_full_honk_relation_row_value(RowEvaluations row_evaluations,
                                                   FF alpha,
                                                   const proof_system::RelationParameters<FF>& relation_parameters);
    // TODO: make this more memory efficient
    static std::vector<FF> compute_level(size_t level,
                                         std::vector<FF> betas,
                                         std::vector<FF> deltas,
                                         std::vector<std::vector<FF>> prev_level_coeffs)
    {
        // if we are at level t in the tree, where t = logn and n is the instance size we have reached the root which
        // contains the coefficients of the perturbator polynomial
        if (level == betas.size()) {
            return prev_level_coeffs[0];
        }

        auto degree = level + 1;
        auto prev_level_width = prev_level_coeffs.size();
        // we need degree + 1 terms to represent intermediate polynomials
        std::vector<std::vector<FF>> level_coeffs(prev_level_width / 2, std::vector<FF>(degree + 1, 0));
        for (size_t node = 0; node < prev_level_width; node += 2) {
            auto parent = node / 2;
            std::copy(prev_level_coeffs[node].begin(), prev_level_coeffs[node].end(), level_coeffs[parent].begin());
            for (size_t d = 0; d < degree; d++) {
                level_coeffs[parent][d] += prev_level_coeffs[node + 1][d] * betas[level];
                level_coeffs[parent][d + 1] += prev_level_coeffs[node + 1][d] * deltas[level];
            }
        }
        return compute_level(level + 1, betas, deltas, level_coeffs);
    }

    static std::vector<FF> construct_perturbator_coeffs(std::vector<FF> betas,
                                                        std::vector<FF> deltas,
                                                        std::vector<FF> full_honk_evaluations)
    {
        // figure out how to make this not take so much memory
        auto width = full_honk_evaluations.size();
        std::vector<std::vector<FF>> first_level_coeffs(width / 2, std::vector<FF>(2, 0));
        for (size_t node = 0; node < width; node += 2) {
            auto idx = node / 2;
            first_level_coeffs[idx][0] = full_honk_evaluations[node] + full_honk_evaluations[node + 1] * betas[0];
            first_level_coeffs[idx][1] = full_honk_evaluations[node + 1] * deltas[0];
        }
        return compute_level(1, betas, deltas, first_level_coeffs);
    }

    // Compute the power perturbator polynomial in coefficient form
    static std::vector<FF> compute_perturbator(std::shared_ptr<Instance> accumulator, std::vector<FF> deltas, FF alpha)
    {
        auto instance_size = accumulator->prover_polynomials[0].size();
        auto const log_instance_size = static_cast<size_t>(numeric::get_msb(instance_size));
        assert(deltas.size() == log_instance_size);

        std::vector<FF> full_honk_evaluations(instance_size);
        for (size_t idx = 0; idx < instance_size; idx++) {

            auto row_evaluations = get_execution_row(accumulator, idx);
            auto full_honk_at_row =
                compute_full_honk_relation_row_value(row_evaluations, alpha, accumulator->relation_parameters);
            full_honk_evaluations[idx] = full_honk_at_row;
        }
        info(full_honk_evaluations);
        auto betas = accumulator->folding_params.gate_separation_challenges;
        assert(betas.size() == log_instance_size);
        return construct_perturbator_coeffs(betas, deltas, full_honk_evaluations);
    }

    ProverFoldingResult<Flavor> fold_instances();
};

extern template class ProtoGalaxyProver_<ProverInstances_<honk::flavor::Ultra, 2>>;
extern template class ProtoGalaxyProver_<ProverInstances_<honk::flavor::UltraGrumpkin, 2>>;
extern template class ProtoGalaxyProver_<ProverInstances_<honk::flavor::GoblinUltra, 2>>;
} // namespace proof_system::honk