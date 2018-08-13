#include "chess.h"
#include <algorithm>
#include <cmath>

namespace Chess {

namespace {

// Move this to a utils file if needed
int normalize(int x)
{
        return x / std::abs(x);
}

bool move_is_valid(Rules const& rules, Side on_turn, Board const& board, Move move)
{
        return std::any_of(rules.cbegin(), rules.cend(),
                [&](Rule const& rule)
                { return rule(on_turn, board, move); }
        );
}

auto direct_pattern(int max_distance = 0)
{
        return [max_distance](Board const& board, Move move)
        {
                auto const check = [&](int a, int b)
                {
                        if (max_distance == 0 || std::abs(a - b) <= max_distance) {
                                for (int i = std::min(a, b);
                                     i < std::max(a, b);
                                     ++i) {
                                        if (board[move.from.x][i])
                                                return false;
                                }
                                return true;
                        }
                        return false;
                };

                if (move.from.x == move.to.x)
                        return check(move.from.y, move.to.y);
                if (move.from.y == move.to.y)
                        return check(move.from.x, move.to.x);
                return false;
        };
}

auto diagonal_pattern()
{
        return [max_distance](Board const& board, Move move)
        {
                auto const [from, to] = move;
                if (std::abs(from.x - to.x) == std::abs(from.y - to.y)) {
                        Point pos = from;
                        int const dx = normalize(to.x - pos.x);
                        int const dy = normalize(to.y - pos.y);
                        while (pos != to) {
                                pos.x += dx;
                                pos.y += dy;
                                if (board[pos.x][pos.y])
                                        return false;
                        }
                        return true;
                }
                return false;
        };
}

auto star_pattern()
{
        return [max_distance](Board const& board, Move move)
        {
                return direct_pattern_pattern()(board, move) ||
                       diagonal_pattern_pattern()(board, move);
        };
}

template <class Callback>
Rule rule(Callback const& callback)
{
        return [callback](Side on_turn, Board const& board, Move move)
        {
                std::optional<Piece> src = board[move.from.x][move.from.y];
                if (!src && src->side == on_turn)
                        return false;
                std::optional<Piece> dst = board[move.to.x][move.to.y];
                return callback(src, dst, board, move);
        };
}

template <class MovementPattern>
Rule movement_rule(Piece::Kind piece_kind, MovementPattern pattern)
{
        return rule(
                [piece_kind, movement_pattern]
                (Piece src, std::optional<Piece> dst,
                 Board const& board, Move move)
                {
                        if (dst && dst->side == src.side)
                                return false;
                }
        );
}

}

bool operator==(Position p1, Position p2) noexcept
{
        return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(Position p1, Position p2) noexcept
{
        return !(p1 == p2);
}

void Move::apply(Board& board) const noexcept
{
        board[to.x][to.y] = board[from.x][from.y];
        board[from.x][from.y] = std::nullopt;
}

void Move::reverse(Board& board) const noexcept
{
        Move const opposite_move { .from = to, .to = from };
        opposite_move.apply(board);
}

Board default_starting_board() noexcept
{
        Board board {0};
        return board;
}

void MoveHistory::add(Move move)
{
        if (last_move_ != moves_.cend())
                moves_.erase(last_move_, moves_.cend());
        moves_.push_back(move);
        last_move_ = moves_.cend();
}

void MoveHistory::undo_move(Board& board) const noexcept
{
        if (last_move_ != moves_.cbegin()) {
                --last_move_;
                last_move_->reverse(board);
        }
}

void MoveHistory::redo_move(Board& board) const noexcept
{
        if (last_move_ != moves_.cend()) {
                last_move_->apply(board);
                ++last_move_;
        }
}

std::vector<Rule> default_rules()
{
        return {
                movement_rule(Piece::Kind::king, direct_pattern(1)),
                movement_rule(Piece::Kind::rook, direct_pattern()),
                movement_rule(Piece::Kind::queen, star_pattern()),
                movement_rule(Piece::Kind::bishop, diagonal_pattern())
        };
}

bool Game::try_move(Move move)
{
        if (move_is_valid(rules_, on_turn_, board_, move)) {
                moves_.push_back(move);
                move.apply(board_);
        }
}



}

