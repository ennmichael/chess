#pragma once

#include <vector>
#include <functional>

namespace Chess {

enum class Side {
        light,
        dark
};

struct Position {
        int x;
        int y;

        friend bool operator==(Position p1, Position p2) noexcept;
        friend bool operator!=(Position p1, Position p2) noexcept;
};

struct Piece {
        enum class Kind {
                king,
                rook,
                queen,
                pawn,
                knight,
                bishop
        };

        Kind kind;
        Side side;
};

unsigned constexpr board_size = 8;

template <class T, auto W, auto H>
using Matrix = std::array<std::array<T, W>, H>;
using Board = Matrix<std::optional<Piece>,
                     board_size, board_size>;

struct Move {
        Point from;
        Point to;

        void apply(Board& board) const noexcept;
        void reverse(Board& board) const noexcept;
};

using Rule = std::function<bool(Side on_turn, Board const& board, Move move)>;

Board default_starting_board() noexcept;
std::vector<Rule> default_rules();

class MoveHistory {
public:
        void add(Move move);
        void undo_move(Board& board) const noexcept;
        void redo_move(Board& board) const noexcept;

private:
        std::vector<Move> moves_;
        std::vector<Move>::const_iterator last_move_ = moves_.cend();
};

class Game {
public:
        bool try_move(Move move);
        void undo();
        void redo();
        Side on_turn() const noexcept;
        Board board() const noexcept;

private:
        Board board_ = default_starting_board();
        std::vector<Rule> rules_ = default_rules();
        MoveHistory move_history_;
        Side on_turn_ = Side::light;
};

class Board {
public:

private:

};

}

