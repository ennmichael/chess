#include "chess.h"
#include <algorithm>
#include <cmath>
#include <cassert>

namespace Chess {

namespace {

int normalize(int x)
{
        return x / std::abs(x);
}

bool move_is_valid(std::vector<Rule> const& rules, Side on_turn,
                   Board const& board, Move move)
{
        return std::any_of(rules.cbegin(), rules.cend(),
                [&](Rule const& rule)
                { return rule(on_turn, board, move); }
        );
}

// Max distance of 0 means there is no distance restriction.
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
                                        if (board[move.from.x][i] != Piece::none())
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
        return [](Board const& board, Move move)
        {
                auto const [from, to] = move;
                if (std::abs(from.x - to.x) == std::abs(from.y - to.y)) {
                        Position pos = from;
                        int const dx = normalize(to.x - pos.x);
                        int const dy = normalize(to.y - pos.y);
                        while (pos != to) {
                                pos.x += dx;
                                pos.y += dy;
                                if (board[pos.x][pos.y] != Piece::none())
                                        return false;
                        }
                        return true;
                }
                return false;
        };
}

auto star_pattern()
{
        return [](Board const& board, Move move)
        {
                return direct_pattern()(board, move) ||
                       diagonal_pattern()(board, move);
        };
}

template <class Callback>
Rule rule(Callback const& callback)
{
        return [callback](Side on_turn, Board const& board, Move move)
        {
                if (on_turn == Side::none)
                        return false;
                Piece src = board[move.from.x][move.from.y];
                if (src.kind == Piece::Kind::none || src.side != on_turn)
                        return false;
                Piece dst = board[move.to.x][move.to.y];
                return callback(src, dst, board, move);
        };
}

template <class MovementPattern>
Rule movement_rule(Piece::Kind piece_kind, MovementPattern pattern)
{
        return rule(
                [piece_kind, pattern](Piece src, Piece dst,
                                      Board const& board, Move move)
                {
                        if (dst.kind != Piece::Kind::none && dst.side == src.side)
                                return false;
                        return pattern(board, move);
                }
        );
}

}

Side opposite_side(Side side) noexcept
{
        assert(side != Side::none);
        return (side == Side::light) ? Side::dark : Side::light;
}

bool operator==(Position p1, Position p2) noexcept
{
        return p1.x == p2.x && p1.y == p2.y;
}

bool operator!=(Position p1, Position p2) noexcept
{
        return !(p1 == p2);
}

bool operator==(Piece p1, Piece p2) noexcept
{
        return p1.kind == p2.kind && p1.side == p2.side;
}

bool operator!=(Piece p1, Piece p2) noexcept
{
        return !(p1 == p2);
}

void Move::apply(Board& board) const noexcept
{
        board[to.x][to.y] = board[from.x][from.y];
        board[from.x][from.y] = Piece::none();
}

void Move::reverse(Board& board) const noexcept
{
        Move const opposite_move {.from = to, .to = from};
        opposite_move.apply(board);
}

Board default_starting_board() noexcept
{
        Board board {Piece::none()};
        return board;
}

void MoveHistory::add_move(Move move, Board& board)
{
        move.apply(board);
        if (last_move_ != moves_.cend())
                moves_.erase(last_move_, moves_.cend());
        moves_.push_back(move);
        last_move_ = moves_.cend();
}

bool MoveHistory::undo_move(Board& board) noexcept
{
        if (last_move_ != moves_.cbegin()) {
                --last_move_;
                last_move_->reverse(board);
                return true;
        }
        return false;
}

bool MoveHistory::redo_move(Board& board) noexcept
{
        if (last_move_ != moves_.cend()) {
                last_move_->apply(board);
                ++last_move_;
                return true;
        }
        return false;
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

// TODO
// Test-run the whole thing
// Write rules for pawn movement, queen movement, castling, en passant

bool Game::try_move(Move move)
{
        if (move_is_valid(rules_, on_turn_, board_, move)) {
                move_history_.add_move(move, board_);
                toggle_turn();
                return true;
        }
        return false;
}

void Game::undo_move()
{
        if (move_history_.undo_move(board_))
                toggle_turn();
}

void Game::redo_move()
{
        if (move_history_.redo_move(board_))
                toggle_turn();
}

Side Game::on_turn() const noexcept
{
        return on_turn_;
}

Board Game::board() const noexcept
{
        return board_;
}

void Game::toggle_turn() noexcept
{
        on_turn_ = opposite_side(on_turn_);
}

}

