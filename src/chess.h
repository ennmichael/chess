#pragma once

#include <vector>
#include <functional>
#include <variant>

/**
 * What's left:
 * test checkmate checking, rule for saying where the king can really jump,
 * mouse control, optionally highlighting valid fields with a macro
 */

namespace Chess {

enum class Side {
        none,
        light,
        dark
};

Side opposite_side(Side side) noexcept;

struct Position {
        int x;
        int y;
};

bool operator==(Position p1, Position p2) noexcept;
bool operator!=(Position p1, Position p2) noexcept;

struct Piece {
        enum class Kind {
                none,
                king,
                rook,
                queen,
                pawn,
                knight,
                bishop
        };

        static Piece constexpr none()
        {
                return Piece {
                        .kind = Kind::none,
                        .side = Side::none
                };
        };
 
        Kind kind;
        Side side;
};

bool operator==(Piece p1, Piece p2) noexcept;
bool operator!=(Piece p1, Piece p2) noexcept;

int constexpr board_size = 8;

template <class T, auto W, auto H>
using Matrix = std::array<std::array<T, W>, H>;
using Board = Matrix<Piece, board_size, board_size>;

struct Move {
        Position from;
        Position to;

        Piece apply(Board& board) const noexcept;
        void undo(Board& board, Piece eaten_piece) const noexcept;
};

class CastlingMove {
public:
        explicit CastlingMove(Move move) noexcept;

        Move rook_move() const noexcept;
        Move king_move() const noexcept;
        void apply(Board& board) const noexcept;
        void undo(Board& board) const noexcept;

private:
        Move rook_move_;
        Move king_move_;
};

class MoveHistory {
public:
        void add_move(Move move, Piece eaten_piece);
        void add_castling_move(CastlingMove castling_move);
        bool undo_move(Board& board) noexcept;
        bool redo_move(Board& board) noexcept;
        bool piece_was_moved(Position piece_position) const noexcept;

private:
        struct NormalMove {
                Move move;
                Piece eaten_piece;
        };

        using Action = std::variant<NormalMove, CastlingMove>;
        using Actions = std::vector<Action>;

        void add_action(Action action);

        Actions actions_;
        Actions::const_iterator last_action_ = actions_.cend();
};

using Rule = std::function<bool(Side on_turn, Board const& board,
                                MoveHistory const& move_history, Move move)>;

Board default_starting_board() noexcept;
std::vector<Rule> default_rules();

using GameOver = void (*)(Side winner);

class Game {
public:
        explicit Game(GameOver game_over) noexcept;

        bool try_move(Move move);
        void undo_move();
        void redo_move();
        Side on_turn() const noexcept;
        Board board() const noexcept;
        Side winner() const noexcept;

private:
        void toggle_turn() noexcept;
        void castling(Move move) noexcept;
        void normal_move(Move move) noexcept;

        GameOver game_over_;
        Board board_ = default_starting_board();
        std::vector<Rule> rules_ = default_rules();
        MoveHistory move_history_;
        Side on_turn_ = Side::light;
};

}

