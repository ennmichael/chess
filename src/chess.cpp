#include "chess.h"
#include <algorithm>
#include <utility>
#include <cmath>
#include <cassert>

namespace Chess {

namespace {

int normalize(int x)
{
        return x / std::abs(x);
}

int constexpr left_rook_x = 0;
int constexpr right_rook_x = 7;
int constexpr left_knight_x = 1;
int constexpr right_knight_x = 6;
int constexpr left_bishop_x = 2;
int constexpr right_bishop_x = 5;
int constexpr queen_x = 3;
int constexpr king_x = 4;

bool move_is_valid(std::vector<Rule> const& rules, Side on_turn,
                   Board const& board, MoveHistory const& move_history,
                   Move move)
{
        return std::any_of(rules.cbegin(), rules.cend(),
                [&](Rule const& rule)
                {
                        return rule(on_turn, board, move_history, move);
                }
        );
}

// Max distance of 0 means there is no distance restriction.
auto direct_pattern(int max_distance = 0) noexcept
{
        return [max_distance](Board const& board, Move move) noexcept
        {
                auto const check = [&](int a, int b, auto piece_at)
                {
                        if (max_distance == 0 || std::abs(a - b) <= max_distance) {
                                for (int i = std::min(a, b) + 1;
                                     i < std::max(a, b);
                                     ++i) {
                                        if (piece_at(i) != Piece::none())
                                                return false;
                                }
                                return true;
                        }
                        return false;
                };

                if (move.from.x == move.to.x) {
                        return check(move.from.y, move.to.y,
                                [&](int i) noexcept
                                {
                                        return board[i][move.from.x];
                                }
                        );
                }

                if (move.from.y == move.to.y) {
                        return check(move.from.x, move.to.x,
                                [&](int i) noexcept
                                {
                                        return board[move.from.y][i];
                                }
                        );
                }

                return false;
        };
}

auto diagonal_pattern(int max_distance = 0) noexcept
{
        return [max_distance](Board const& board, Move move) noexcept
        {
                auto const [from, to] = move;
                int const d = std::abs(from.x - to.x);
                if (d == std::abs(from.y - to.y) &&
                    (max_distance == 0 || d <= max_distance)) {
                        int const dx = normalize(to.x - from.x);
                        int const dy = normalize(to.y - from.y);
                        Position pos {from.x + dx, from.y + dy};
                        while (pos != to) {
                                if (board[pos.y][pos.x] != Piece::none())
                                        return false;
                                pos.x += dx;
                                pos.y += dy;
                        }
                        return true;
                }
                return false;
        };
}

auto star_pattern(int max_distance = 0) noexcept
{
        return [max_distance](Board const& board, Move move) noexcept
        {
                return direct_pattern(max_distance)(board, move) ||
                       diagonal_pattern(max_distance)(board, move);
        };
}

// TODO Refactor: add kind_rule

template <class Callback>
Rule rule(Callback const& callback)
{
        return [callback](Side on_turn, Board const& board,
                          MoveHistory const& move_history, Move move)
        {
                if (on_turn == Side::none)
                        return false;
                Piece src = board[move.from.y][move.from.x];
                if (src.kind == Piece::Kind::none || src.side != on_turn)
                        return false;
                Piece dst = board[move.to.y][move.to.x];
                return callback(src, dst, board, move_history, move);
        };
}

template <class MovementPattern>
Rule movement_rule(Piece::Kind piece_kind, MovementPattern pattern)
{
        return rule(
                [piece_kind, pattern](Piece src, Piece dst, Board const& board,
                                      MoveHistory const&, Move move)
                {
                        if (dst.side == src.side || src.kind != piece_kind)
                                return false;
                        return pattern(board, move);
                }
        );
}

Rule king_movement_rule()
{
        return movement_rule(Piece::Kind::king, star_pattern(1));
}

Rule rook_movement_rule()
{
        return movement_rule(Piece::Kind::rook, direct_pattern());
}

Rule queen_movement_rule()
{
        return movement_rule(Piece::Kind::queen, star_pattern());
}

Rule bishop_movement_rule()
{
        return movement_rule(Piece::Kind::bishop, diagonal_pattern());
}

Rule pawn_movement_rule()
{
        return rule(
                [](Piece src, Piece dst, Board const& board,
                   MoveHistory const& move_history, Move move)
                {
                        if (src.kind != Piece::Kind::pawn || dst.side == src.side)
                                return false;

                        int const dx = move.to.x - move.from.x;
                        int const dy = move.to.y - move.from.y;
                        int const y_direction = (src.side == Side::light) ? -1 : 1;
                        if (dst != Piece::none()) {
                                // En passant?
                                return dy == y_direction && std::abs(dx) == 1;
                        } else if (dx == 0) {
                                bool const piece_was_moved =
                                        move_history.piece_was_moved(move.from);
                                bool const blocked =
                                        board[move.from.y + y_direction][move.from.x] !=
                                        Piece::none();
                                return dy == y_direction ||
                                       (!piece_was_moved &&
                                        !blocked &&
                                        dy == y_direction * 2);
                        }

                        return false;
                }
        );
}

Rule knight_movement_rule()
{
        return rule(
                [](Piece src, Piece dst, Board const&, MoveHistory const&, Move move)
                {
                        if (src.kind != Piece::Kind::knight || dst.side == src.side)
                                return false;
                        int const dx = std::abs(move.from.x - move.to.x);
                        int const dy = std::abs(move.from.y - move.to.y);
                        return (dx == 1 && dy == 2) || (dx == 2 && dy == 1);
                }
        );
}

int home_rank_y(Side side) noexcept
{
        assert(side != Side::none);
        return (side == Side::light) ? board_size - 1 : 0;
}

int pawn_rank_y(Side side) noexcept
{
        assert(side != Side::none);
        return (side == Side::light) ? board_size - 2 : 1;
}

Position castling_rook_position(Move move) noexcept
{
        return (move.to.x == king_x) ? move.from : move.to;
}

Position castling_king_position(Move move) noexcept
{
        return (move.to.x == king_x) ? move.to : move.from;
}

Rule castling_rule()
{
        return rule(
                [](Piece src, Piece dst, Board const& board,
                   MoveHistory const& move_history, Move move)
                {
                        auto const king_or_rook =
                        [](Piece const piece)
                        {
                                return piece.kind == Piece::Kind::king ||
                                       piece.kind == Piece::Kind::rook;
                        };

                        if (src.side != dst.side ||
                            !king_or_rook(src) ||
                            !king_or_rook(dst) ||
                            move_history.piece_was_moved(move.from) ||
                            move_history.piece_was_moved(move.to)) {
                                return false;
                        }

                        auto const [x, y] = castling_rook_position(move);
                        if (x == left_rook_x) {
                                return board[y][left_knight_x] == Piece::none() &&
                                       board[y][left_bishop_x] == Piece::none() &&
                                       board[y][queen_x] == Piece::none();
                        } else if (x == right_rook_x) {
                                return board[y][right_bishop_x] == Piece::none() &&
                                       board[y][right_knight_x] == Piece::none();
                        } else {
                                return false;
                        }
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

Piece Move::apply(Board& board) const noexcept
{
        auto const eaten_piece = board[to.y][to.x];
        board[to.y][to.x] = board[from.y][from.x];
        board[from.y][from.x] = Piece::none();
        return eaten_piece;
}

void Move::undo(Board& board, Piece eaten_piece) const noexcept
{
        Move const opposite_move {.from = to, .to = from};
        opposite_move.apply(board);
        board[to.y][to.x] = eaten_piece;
}

CastlingMove::CastlingMove(Move move) noexcept
{
        auto const rook_position = castling_rook_position(move);
        auto const king_position = castling_king_position(move);
        if (rook_position.x == left_rook_x) {
                rook_move_ = Move {
                        .from = rook_position,
                        .to = Position {queen_x, rook_position.y}
                };
                king_move_ = Move {
                        .from = king_position,
                        .to = Position {left_bishop_x, king_position.y}
                };
        } else if (rook_position.x == right_rook_x) {
                rook_move_ = Move {
                        .from = rook_position,
                        .to = Position {right_bishop_x, rook_position.y}
                };
                king_move_ = Move {
                        .from = king_position,
                        .to = Position {right_knight_x, king_position.y}
                };
        } else {
                assert(false);
        }
}

Move CastlingMove::rook_move() const noexcept
{
        return rook_move_;
}

Move CastlingMove::king_move() const noexcept
{
        return king_move_;
}

void CastlingMove::apply(Board& board) const noexcept
{
        rook_move_.apply(board);
        king_move_.apply(board);
}

void CastlingMove::undo(Board& board) const noexcept
{
        rook_move_.undo(board, Piece::none());
        king_move_.undo(board, Piece::none());
}

void MoveHistory::add_move(Move move, Piece eaten_piece)
{
        add_action(NormalMove {move, eaten_piece});
}

void MoveHistory::add_castling_move(CastlingMove castling_move)
{
        add_action(castling_move);
}

bool MoveHistory::undo_move(Board& board) noexcept
{
        struct UndoVisitor {
                Board& board;

                void operator()(NormalMove normal_move) const noexcept
                {
                        normal_move.move.undo(board, normal_move.eaten_piece);
                }

                void operator()(CastlingMove castling_move) const noexcept
                {
                        castling_move.undo(board);
                }
        };

        if (last_action_ != actions_.cbegin()) {
                --last_action_;
                std::visit(UndoVisitor {board}, *last_action_);
                return true;
        }
        return false;
}

bool MoveHistory::redo_move(Board& board) noexcept
{
        struct RedoVisitor {
                Board& board;

                void operator()(NormalMove normal_move) const noexcept
                {
                        normal_move.move.apply(board);
                }

                void operator()(CastlingMove castling_move) const noexcept
                {
                        castling_move.apply(board);
                }
        };

        if (last_action_ != actions_.cend()) {
                std::visit(RedoVisitor {board}, *last_action_);
                ++last_action_;
                return true;
        }
        return false;
}

bool MoveHistory::piece_was_moved(Position piece_position) const noexcept
{
        struct Visitor {
                Position piece_position;

                bool operator()(NormalMove normal_move) const noexcept
                {
                        return normal_move.move.to == piece_position;
                }

                bool operator()(CastlingMove castling_move) const noexcept
                {
                        return castling_move.king_move().to == piece_position ||
                               castling_move.rook_move().to == piece_position;
                }
        };

        return std::any_of(actions_.cbegin(), actions_.cend(),
                [&](Action action) noexcept
                {
                        return std::visit(Visitor {piece_position}, action);
                }
        );
}

void MoveHistory::add_action(Action action)
{
        if (last_action_ != actions_.cend())
                actions_.erase(last_action_, actions_.cend());
        actions_.push_back(std::move(action));
        last_action_ = actions_.cend();
}

Board default_starting_board() noexcept
{
        Board board {Piece::none()};

        auto const add_pawn_rank =
        [&](Side side) noexcept
        {
                int const y = pawn_rank_y(side);
                for (int x = 0; x < board_size; ++x) {
                        board[y][x] = Piece {
                                .kind = Piece::Kind::pawn,
                                .side = side
                        };
                }
        };

        auto const add_home_rank =
        [&](Side side) noexcept
        {
                int const y = home_rank_y(side);
                board[y][left_rook_x] = board[y][right_rook_x] = Piece {
                        .kind = Piece::Kind::rook,
                        .side = side
                };
                /*board[y][left_knight_x] = board[y][right_knight_x] = Piece {
                        .kind = Piece::Kind::knight,
                        .side = side
                };
                board[y][left_bishop_x] = board[y][right_bishop_x] = Piece {
                        .kind = Piece::Kind::bishop,
                        .side = side
                };
                board[y][queen_x] = Piece {
                        .kind = Piece::Kind::queen,
                        .side = side
                };*/
                board[y][king_x] = Piece {
                        .kind = Piece::Kind::king,
                        .side = side
                };
        };

        add_home_rank(Side::dark);
        add_pawn_rank(Side::dark);
        add_pawn_rank(Side::light);
        add_home_rank(Side::light);
        return board;
}

std::vector<Rule> default_rules()
{
        return {
                king_movement_rule(),
                rook_movement_rule(),
                queen_movement_rule(),
                bishop_movement_rule(),
                pawn_movement_rule(),
                knight_movement_rule(),
                castling_rule()
        };
}

bool Game::try_move(Move move)
{
        if (move_is_valid(rules_, on_turn_, board_, move_history_, move)) {
                Piece const src = board_[move.from.y][move.from.x];
                Piece const dst = board_[move.to.y][move.to.x];
                if (src.side == dst.side)
                        castling(move);
                else
                        normal_move(move);
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

void Game::castling(Move move) noexcept
{
        CastlingMove castling_move(move);
        castling_move.apply(board_);
        move_history_.add_castling_move(castling_move);
}

void Game::normal_move(Move move) noexcept
{
        auto const eaten_piece = move.apply(board_);
        move_history_.add_move(move, eaten_piece);
}

}

