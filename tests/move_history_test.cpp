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
        Piece const dark_pawn {
                .kind = Piece::Kind::pawn,
                .side = Side::light
        };
        board[1][1] = light_pawn;
        board[5][5] = dark_pawn;

        auto const check_piece =
        [&](Piece piece, Position pos) noexcept
        {
                for (int x = 0; x < board_size; ++x) {
                        for (int y = 0; y < board_size; ++y) {
                                if (pos == Position {x, y})
                                        CHECK(board[y][x] == piece);
                                else
                                        CHECK(board[y][x] == Piece::none());
                        }
                }
        };

        auto const check_light_pawn =
        [&](Position pos) noexcept
        {
                return check_piece(light_pawn, pos);
        };

        auto const check_no_dark_pawn =
        [&]
        {
                for (int x = 0; x < board_size; ++x) {
                        for (int y = 0; y < board_size; ++y) {
                                CHECK(board[y][x] != dark_pawn);
                        }
                }
        };

        auto const check_dark_pawn =
        [&](Position pos) noexcept
        {
                return check_piece(dark_pawn, pos);
        };
        
        MoveHistory history;

        auto const add_move =
        [&](Move move)
        {
                auto const eaten_piece = move.apply(board);
                history.add_move(move, eaten_piece);
        };

        add_move(Move {.from = {1, 1}, .to = {5, 5}});
        add_move(Move {.from = {5, 5}, .to = {5, 3}});
        add_move(Move {.from = {5, 3}, .to = {4, 2}});
        add_move(Move {.from = {4, 2}, .to = {4, 5}});
        check_light_pawn({4, 5});
        check_no_dark_pawn();

        CHECK(!history.redo_move(board));
        check_light_pawn({4, 5});
        check_no_dark_pawn();

        CHECK(history.undo_move(board));
        check_light_pawn({4, 2});
        check_no_dark_pawn();

        CHECK(history.undo_move(board));
        check_light_pawn({5, 3});
        check_no_dark_pawn();
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 5});
        check_no_dark_pawn();
        
        CHECK(history.redo_move(board));
        check_light_pawn({5, 3});
        check_no_dark_pawn();
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 5});
        check_no_dark_pawn();

        CHECK(history.undo_move(board));
        check_light_pawn({1, 1});
        check_dark_pawn({5, 5});
        
        CHECK(history.redo_move(board));
        check_light_pawn({5, 5});
        check_no_dark_pawn();

        CHECK(history.redo_move(board));
        check_light_pawn({5, 3});
        check_no_dark_pawn();
        
        add_move(Move {.from = {5, 3}, .to = {3, 3}});
        check_light_pawn({3, 3});
        check_no_dark_pawn();
        
        CHECK(!history.redo_move(board));
        check_light_pawn({3, 3});
        check_no_dark_pawn();
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 3});
        check_no_dark_pawn();
        
        CHECK(history.undo_move(board));
        check_light_pawn({5, 5});
        check_no_dark_pawn();

        CHECK(history.undo_move(board));
        check_light_pawn({1, 1});
        check_no_dark_pawn();

        CHECK(!history.undo_move(board));
        check_light_pawn({1, 1});
        check_no_dark_pawn();
}

