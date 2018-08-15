#include "castling.h"

namespace Chess {

namespace {

Position castling_rook_position(Move move) noexcept
{}

Position castling_king_position(Move move) noexcept
{}

}

std::pair<Position> rook_and_king_positions(Move move) noexcept
{
        auto const rook_position = 
                (move.from.x == left_rook_x || move.from.x == right_rook_x) ?
                move.from : move.to;
        auto const king_position = (move.from.x == king_x) ? move.from : move.to;
        return std::pair<Position>(rook_position, king_position);
}

// TODO Just use std::pair
struct CastlingMoves {
        explicit CastlingMoves(Move move) noexcept
        {
                auto const [rook_position, king_position] = rook_and_king_positions(move);
                if (rook_position.x == left_rook_x) {
                        rook_move = Move {
                                .from = rook_position,
                                .to = Position {queen_x, rook_position.y}
                        };
                        king_move = Move {
                                .from = king_position,
                                .to = Position {left_bishop_x, king_position.y}
                        };
                } else if (rook_position.x == right_rook_x) {
                        rook_move = Move {
                                .from = rook_position,
                                .to = Position {right_bishop_x, rook_position.y}
                        };
                        king_move = Move {
                                .from = king_position,
                                .to = Position {right_knight_x, king_position.y}
                        };
                } else {
                        assert(false);
                }
        }

        Move rook_move;
        Move king_move;
};

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

void castling(Board& board, Move move) noexcept
{
        Piece& src = board[move.from.y][move.from.x];
        Piece& dst = board[move.to.y][move.to.x];
        int const y = home_rank_y(side);
        src = dst = Piece::none();
        Piece const king {.kind = Piece::Kind::king, .side = src.side};
        Piece const rook {.kind = Piece::Kind::rook, .side = src.side};
        if (rook_x == left_rook_x) {
                board[y][left_bishop_x] = king;
                board[y][queen_x] = rook;
        } else if (rook_x == right_rook_x) {
                board[y][right_knight_x] = king;
                board[y][right_bishop_x] = rook;
        } else {
                assert(false);
        }
}

// TODO This is incorrect
void undo_castling(Board& board, Move move) noexcept
{
        int const y = home_rank_y(side);
        int const rook_x = (src.kind == Piece::Kind::rook) ?
                move.from.x : move.to.x;
        board[y][king_x] = Piece {
                .kind = Piece::Kind::king,
                .side = side
        };
        board[y][rook_x] = Piece {
                .kind = Piece::Kind::rook,
                .side = side
        };
        if (rook_x == left_rook_x) {
                board[y][left_bishop_x] = Piece::none();
                board[y][queen_x] = Piece::none();
        } else if (rook_x == right_rook_x) {
                board[y][right_bishop_x] = Piece::none();
                board[y][right_knight_x] = Piece::none();
        } else {
                assert(false);
        }
}

}

