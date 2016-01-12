#include "pairwise_potential.hpp"

double PairwisePotential::calculate_energy(const Space& space) const {
    double energy(0);
    if(std::shared_ptr<NeighborList> neighbor_list = _get_neighbor_list()) {
        const std::size_t size(neighbor_list->size());
        for (std::size_t i(0); i < size; ++i) {
            const Vector3d coord(space.coordinate(i));
            for (auto itr(neighbor_list->begin(i)); itr != neighbor_list->end(i); ++itr) {
                energy += calculate_unit_energy(norm(space.coordinate(*itr) - coord));
            }
        }
    } else {
        const std::size_t num_beads(space.num_beads());
        for (std::size_t i(0); i < num_beads-1; ++i) {
            const std::string symbol0(space.symbol(i));
            if (symbol0 != _pair.first && symbol0 != _pair.second)
                continue;
            const Vector3d coord(space.coordinate(i));
            for (std::size_t j(i+1); j < num_beads; ++j) {
                if (is_valid_pair(symbol0, space.symbol(j)))
                    energy += calculate_unit_energy(norm(space.coordinate(j) - coord));
            }
        }
    }
    return energy;
}

vector_list PairwisePotential::calculate_force(const Space& space) const {
    const std::size_t num_beads(space.num_beads());
    vector_list forces(num_beads, Vector3d(0,0,0));
    if(std::shared_ptr<NeighborList> neighbor_list = _get_neighbor_list()) {
        const std::size_t size(neighbor_list->size());
        for (std::size_t i(0); i < size; ++i) {
            const Vector3d coord(space.coordinate(i));
            for (auto itr(neighbor_list->begin(i)); itr != neighbor_list->end(i); ++itr) {
                const Vector3d force(calculate_unit_force(space.coordinate(*itr) - coord));
                forces.at(i) -= force;
                forces.at(*itr) += force;
            }
        }
    } else {
        for (std::size_t i(0); i < num_beads-1; ++i) {
            const std::string symbol0(space.symbol(i));
            if (symbol0 != _pair.first && symbol0 != _pair.second)
                continue;
            const Vector3d coord(space.coordinate(i));
            for (std::size_t j(i+1); j < num_beads; ++j) {
                if (is_valid_pair(symbol0, space.symbol(j))) {
                    const Vector3d force(calculate_unit_force(space.coordinate(j) - coord));
                    forces.at(i) -= force;
                    forces.at(j) += force;
                }
            }
        }
    }
    return forces;
}

void PairwisePotential::set_neighbor_list(std::shared_ptr<NeighborList> neighbor_list) {
    _neighbor_list = neighbor_list;
}

std::shared_ptr<NeighborList> PairwisePotential::_get_neighbor_list() const {
    return _neighbor_list.lock();
}

void PairwisePotential::set_pair(std::string symbol0, std::string symbol1) {
    _pair = std::make_pair(symbol0, symbol1);
}

std::pair<std::string, std::string> PairwisePotential::get_pair() const {
    return _pair;
}