#include "catch.hpp"
#include "chess.h"

TEST_CASE("Move history works")
{
        using namespace Chess;

        Board board {Piece::none()};
        Piece const light_pawn {
                .kind = Piece::Kind::pawn,
                .side = Side::light
        };
        board[1][1] = light_pawn;

        auto const check_light_pawn =
        [&](Position pos)
        {
                for (int x = 0; x < board_size; ++x) {
                        for (int y = 0; y < board_size; ++y) {
                                if (pos == Position {x, y})
                                        CHECK(board[x][y] == light_pawn);
                                else
                                        CHECK(board[x][y] == Piece::none());
                        }
                }
        };

        MoveHistory history;
        history.add_move(Move {.from = {1, 1}, .to = {5, 5}}, board);
        history.add_move(Move {.from = {5, 5}, .to = {5, 3}}, board);
        history.add_move(Move {.from = {5, 3}, .to = {4, 2}}, board);
        history.add_move(Move {.from = {4, 2}, .to = {4, 5}}, board);
        check_light_pawn({4, 5});

        CHECK(!history.redo_move(board));
        check_light_pawn({4, 5});

        CHECK(history.undo_move(board));
        check_light_pawn({4, 2});

        CHECK(history.undo_move(board));
        check_light_pawn({5, 3});
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 5});
        
        CHECK(history.redo_move(board));
        check_light_pawn({5, 3});
        
        history.add_move(Move {.from = {5, 3}, .to = {3, 3}}, board);
        check_light_pawn({3, 3});
        
        CHECK(!history.redo_move(board));
        check_light_pawn({3, 3});
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 3});
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 5});

        CHECK(history.undo_move(board));
        check_light_pawn({1, 1});

        CHECK(!history.undo_move(board));
        check_light_pawn({1, 1});
}

