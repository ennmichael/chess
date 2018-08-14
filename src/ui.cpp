#include "ui.h"
#include <cassert>

namespace Chess {

PieceSelector::PieceSelector(Position pos) noexcept
        : current_position_(pos)
{}

std::optional<Move> PieceSelector::select() noexcept
{
        if (!selected_position_) {
                selected_position_ = current_position_;
                return std::nullopt;
        } else  {
                Move move {.from = *selected_position_, .to = current_position_};
                selected_position_ = std::nullopt;
                return move;
        }
}

void PieceSelector::move_up() noexcept
{
        current_position_.y -= 1;
        wrap_position();
}

void PieceSelector::move_down() noexcept
{
        current_position_.y += 1;
        wrap_position();
}

void PieceSelector::move_left() noexcept
{
        current_position_.x -= 1;
        wrap_position();
}

void PieceSelector::move_right() noexcept
{
        current_position_.x += 1;
        wrap_position();
}

Position PieceSelector::current_position() const noexcept
{
        return current_position_;
}

std::optional<Position> PieceSelector::selected_position() const noexcept
{
        return selected_position_;
}

void PieceSelector::wrap_position() noexcept
{
        auto const wrap = [](int& value)
        {
                if (value < 0)
                        value = board_size + (value % board_size);
                else
                        value = value % board_size;
        };

        wrap(current_position_.x);
        wrap(current_position_.y);
}

}

