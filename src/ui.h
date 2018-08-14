#pragma once

#include "chess.h"
#include <optional>

namespace Chess {

class PieceSelector {
public:
        explicit PieceSelector(Position pos) noexcept;

        std::optional<Move> select() noexcept;
        void move_up() noexcept;
        void move_down() noexcept;
        void move_left() noexcept;
        void move_right() noexcept;
        Position current_position() const noexcept;
        std::optional<Position> selected_position() const noexcept;

private:
        void wrap_position() noexcept;

        Position current_position_;
        std::optional<Position> selected_position_ = std::nullopt;
};

}

