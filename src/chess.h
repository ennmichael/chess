#pragma once

#include <vector>
#include <functional>

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

        void apply(Board& board) const noexcept;
        void reverse(Board& board) const noexcept;
};

// TODO Here we'll also need to look at the move history, for example controlling a pawn.
using Rule = std::function<bool(Side on_turn, Board const& board, Move move)>;

Board default_starting_board() noexcept;
std::vector<Rule> default_rules();

// TODO This class can have some more useful functions,
// like checking if a piece was ever moved from a position
class MoveHistory {
public:
        void add_move(Move move, Board& board);
        bool undo_move(Board& board) noexcept;
        bool redo_move(Board& board) noexcept;

private:
        std::vector<Move> moves_;
        std::vector<Move>::const_iterator last_move_ = moves_.cend();
};

class Game {
public:
        bool try_move(Move move);
        void undo_move();
        void redo_move();
        Side on_turn() const noexcept;
        Board board() const noexcept;

private:
        void toggle_turn() noexcept;

        Board board_ = default_starting_board();
        std::vector<Rule> rules_ = default_rules();
        MoveHistory move_history_;
        Side on_turn_ = Side::light;
};

}

