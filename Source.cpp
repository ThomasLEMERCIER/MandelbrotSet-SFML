/*
MIT License

Copyright (c) 2021 LEMERCIER Thomas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <complex>
#include <immintrin.h>
#include <thread>
#include "input_system.h"
#include "thread_pool.h"

const int WIDTH = 1280; const int HEIGHT = 880; const int THREAD_NUMBER = 32;

void compute_fractal_basic(sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void compute_fractal_increment(sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void compute_fractal_no_complex(sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void compute_fractal_intrinsic(sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void compute_fractal_multithreading(sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void compute_fractal_threadpool(ThreadPool*, sf::Vector2<double>, sf::Vector2<double>, sf::Vector2<int>, sf::Vector2<int>, int, sf::Image*);
void initialise_graphics(sf::RenderWindow*, sf::Image*, sf::Font*, sf::Text*);
void initialise_boundary(sf::Vector2<double>&, sf::Vector2<double>&, sf::Vector2<int>&, sf::Vector2<int>&);
sf::Color get_color(int);

int main()
{

    std::cout << "Credit to Thomas L.\nNavigate using the mouse, Zoom in using A and zoom out using E, reset the view using Space\n";
    std::cout << "Different implementation to compute the mandelbrot set are usable to switch use the Numpad from 0 to 5.\n";
    std::cout << "You cannot zoom in indefinitly as the computer will run out of precision using 64 bits\n";
    std::cout << "Press Enter to Launch the render window.";


    std::cin.get();


    sf::Font font;
    sf::Text text;
    sf::RenderWindow window;

    sf::Image fractal; fractal.create(WIDTH, HEIGHT);
    sf::Texture texture;
    sf::Sprite sprite;

    std::string string_display;
    std::string compute_method;

    int iterations = 16;

    sf::Vector2<double> frac_tl;
    sf::Vector2<double> frac_br;

    sf::Vector2<int> pixel_tl;
    sf::Vector2<int> pixel_br;

    int n_method = 0;

    input_system input_sys(&window);

    initialise_graphics(&window, &fractal, &font, &text);
    initialise_boundary(frac_tl, frac_br, pixel_tl, pixel_br);

    ThreadPool Tpool(THREAD_NUMBER);

    while (window.isOpen())
    {
        // Event loop 
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // User Input
        auto begin_input = std::chrono::high_resolution_clock::now();
        input_sys.handle_move(frac_tl, frac_br);
        input_sys.handle_precision(iterations);
        input_sys.handle_zoom(frac_tl, frac_br);
        n_method = input_sys.handle_method();
        auto end_input = std::chrono::high_resolution_clock::now();

        // Compute
        switch (n_method)
        {
        case 0:
            compute_fractal_basic(frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Basic Computation";
            break;
        case 1:
            compute_fractal_increment(frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Computation by incrementation";
            break;
        case 2:
            compute_fractal_no_complex(frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Computation without standard complex";
            break;
        case 3:
            compute_fractal_intrinsic(frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Computation using AVX2";
            break;
        case 4:
            compute_fractal_multithreading(frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Computation using Multithreading";
            break;
        case 5:
            compute_fractal_threadpool(&Tpool, frac_tl, frac_br, pixel_tl, pixel_br, iterations, &fractal);
            compute_method = "Computation using threadpoool";
            break;
        default:
            compute_method = "Not computing";
            break;
        }
        auto end_compute = std::chrono::high_resolution_clock::now();

        // Draw
        texture.loadFromImage(fractal); sprite.setTexture(texture);
        window.draw(sprite);
        auto end_draw = std::chrono::high_resolution_clock::now();

        // Result
        std::chrono::duration<double> elapsedTime_input = end_input - begin_input;
        std::chrono::duration<double> elapsedTime_compute = end_compute - end_input;
        std::chrono::duration<double> elapsedTime_draw = end_draw - end_compute;

        string_display = "Time Taken input: " +std::to_string(elapsedTime_input.count()) + "s\n";
        string_display += "Time Taken compute: " + std::to_string(elapsedTime_compute.count()) + "s\n";
        string_display += "Time Taken draw: " + std::to_string(elapsedTime_draw.count()) + "s\n";
        string_display += "Iterations: " + std::to_string(iterations) + "\n";
        string_display += compute_method;
        text.setString(string_display);

        window.draw(text);

        // end the current frame
        window.display();
    }
}

void initialise_graphics(sf::RenderWindow* renderer, sf::Image* image, sf::Font* font, sf::Text* text)
{
    renderer->create(sf::VideoMode(WIDTH, HEIGHT), "Mandelbrot Set", sf::Style::Titlebar | sf::Style::Close);
    font->loadFromFile("arial.ttf");
    image->create(WIDTH, HEIGHT);
    text->setFont(*font);
    text->setCharacterSize(18);
    text->setFillColor(sf::Color::Black);
}

void initialise_boundary(sf::Vector2<double>& frac_tl, sf::Vector2<double>& frac_br, sf::Vector2<int>& pixel_tl, sf::Vector2<int>& pixel_br)
{
    frac_tl = { -2., 1. };
    frac_br = { 1., -1. };
    pixel_tl = { 0,0 };
    pixel_br = { WIDTH, HEIGHT };
}

sf::Color get_color(int iteration)
{
    float a = 0.1f;
    return sf::Color((int)(255 * (0.5f * sin(a * iteration) + 0.5f)), (int)(255 * (0.5f * sin(a * iteration + 2.094f) + 0.5f)), (int)(255 * (0.5f * sin(a * iteration + 4.188f) + 0.5f)));
}

void compute_fractal_basic(sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{

    double x_ratio = (frac_br.x - frac_tl.x) / ((double)pixel_br.x - (double)pixel_tl.x);
    double y_ratio = (frac_br.y - frac_tl.y) / ((double)pixel_br.y - (double)pixel_tl.y);

    for (int y = pixel_tl.y; y < pixel_br.y; y++)
    {
        for (int x = pixel_tl.x; x < pixel_br.x; x++)
        {
            std::complex<double> z(0., 0.);
            std::complex<double> c(x * x_ratio + frac_tl.x, y * y_ratio + frac_tl.y);
            int n = 0;

            while (abs(z) <= 2. && n < iterations)
            {
                z = z * z + c;
                n++;
            }
            fractal->setPixel(x, y, get_color(n));
        }
    }
}

void compute_fractal_increment(sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{

    double x_ratio = (frac_br.x - frac_tl.x) / ((double)pixel_br.x - (double)pixel_tl.x);
    double y_ratio = (frac_br.y - frac_tl.y) / ((double)pixel_br.y - (double)pixel_tl.y);

    int x, y;

    double ci = frac_tl.y;

    for (y = pixel_tl.y; y < pixel_br.y; y++)
    {
        double cr = frac_tl.x;
        for (x = pixel_tl.x; x < pixel_br.x; x++)
        {
            std::complex<double> z(0., 0.);
            std::complex<double> c(cr, ci);
            int n = 0;

            while (abs(z) <= 2. && n < iterations)
            {
                z = z * z + c;
                n++;
            }

            fractal->setPixel(x, y, get_color(n));

            cr += x_ratio;
        }
        ci += y_ratio;
    }
}

void compute_fractal_no_complex(sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{

    double x_ratio = (frac_br.x - frac_tl.x) / ((double)pixel_br.x - (double)pixel_tl.x);
    double y_ratio = (frac_br.y - frac_tl.y) / ((double)pixel_br.y - (double)pixel_tl.y);

    int n = 0;

    double cr = 0;
    double ci = frac_tl.y;
    double zr = 0;
    double zi = 0;
    double zr_temp;

    for (int y = pixel_tl.y; y < pixel_br.y; y++)
    {
        cr = frac_tl.x;
        for (int x = pixel_tl.x; x < pixel_br.x; x++)
        {
            zr = 0;
            zi = 0;
            n = 0;

            while (zr*zr+zi*zi <= 4. && n < iterations)
            {
                zr_temp = zr * zr - zi * zi + cr;
                zi = 2 * zr * zi + ci;
                zr = zr_temp;
                n++;
            }

            fractal->setPixel(x, y, get_color(n));

            cr += x_ratio;
        }
        ci += y_ratio;
    }
}

void compute_fractal_intrinsic(sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{

    double x_ratio = (frac_br.x - frac_tl.x) / ((double)pixel_br.x - (double)pixel_tl.x);
    double y_ratio = (frac_br.y - frac_tl.y) / ((double)pixel_br.y - (double)pixel_tl.y);

    __m256i _n, _iterations, _mask_iterations, _c, _one;
    __m256d _cr, _ci, _zr, _zi, _a, _b, _x_offset, _x_jump, _zr2, _zi2, _two, _four, _mask_bound;

    double ci = frac_tl.y;

    _iterations = _mm256_set1_epi64x(iterations);
    _one = _mm256_set1_epi64x(1);

    _two = _mm256_set1_pd(2.);
    _four = _mm256_set1_pd(4.);

    _x_offset = _mm256_set_pd(0., x_ratio, 2 * x_ratio, 3 * x_ratio);
    _x_jump = _mm256_set1_pd(4 * x_ratio);


    for (int y = pixel_tl.y; y < pixel_br.y; y++)
    {
        _ci = _mm256_set1_pd(ci);

        _cr = _mm256_set1_pd(frac_tl.x);
        _cr = _mm256_add_pd(_cr, _x_offset);


        for (int x = pixel_tl.x; x < pixel_br.x; x += 4)
        {
            
            _zr = _mm256_setzero_pd();
            _zi = _mm256_setzero_pd();
            _n = _mm256_setzero_si256();
            
        repeat:

            _zr2 = _mm256_mul_pd(_zr, _zr);
            _zi2 = _mm256_mul_pd(_zi, _zi);
            _a = _mm256_sub_pd(_zr2, _zi2);
            _a = _mm256_add_pd(_a, _cr);
            _b = _mm256_mul_pd(_zr, _zi);
            _b = _mm256_fmadd_pd(_b, _two, _ci);
            _zr = _a;
            _zi = _b;
            _a = _mm256_add_pd(_zr2, _zi2);
            _mask_bound = _mm256_cmp_pd(_a, _four, _CMP_LE_OQ);
            _mask_iterations = _mm256_cmpgt_epi64(_iterations, _n);
            _mask_iterations = _mm256_and_si256(_mask_iterations, _mm256_castpd_si256(_mask_bound));
            _c = _mm256_and_si256(_one, _mask_iterations);
            _n = _mm256_add_epi64(_n, _c);

            if (_mm256_movemask_pd(_mm256_castsi256_pd(_mask_iterations)) > 0)
                goto repeat;

            fractal->setPixel(x, y, get_color(int(_n.m256i_i64[3])));
            fractal->setPixel(x+1, y, get_color(int(_n.m256i_i64[2])));
            fractal->setPixel(x+2, y, get_color(int(_n.m256i_i64[1])));
            fractal->setPixel(x+3, y, get_color(int(_n.m256i_i64[0])));
            
            _cr = _mm256_add_pd(_cr, _x_jump);

        }
        ci += y_ratio;
    }
}

void compute_fractal_multithreading(sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{
    int spixel_width = (pixel_br.x - pixel_tl.x) / THREAD_NUMBER;
    double sfractal_width = (frac_br.x - frac_tl.x) / THREAD_NUMBER;


    std::thread t[THREAD_NUMBER];

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        t[i] = std::thread(compute_fractal_intrinsic, sf::Vector2<double>(frac_tl.x + double(i) * sfractal_width, frac_tl.y),
            sf::Vector2<double>(frac_tl.x + (double(i) + 1) * sfractal_width, frac_br.y),
            sf::Vector2<int>(pixel_tl.x + i * spixel_width, pixel_tl.y),
            sf::Vector2<int>(pixel_tl.x + (i + 1) * spixel_width, pixel_br.y), iterations, fractal);
    }

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        t[i].join();
    }
}

void compute_fractal_threadpool(ThreadPool* pool,sf::Vector2<double> frac_tl, sf::Vector2<double> frac_br, sf::Vector2<int> pixel_tl, sf::Vector2<int> pixel_br, int iterations, sf::Image* fractal)
{
    int spixel_width = (pixel_br.x - pixel_tl.x) / THREAD_NUMBER;
    double sfractal_width = (frac_br.x - frac_tl.x) / THREAD_NUMBER;

    std::atomic<int> ctr = 0;

    for (int i = 0; i < THREAD_NUMBER; i++)
    {
        auto result = pool->enqueue([=, &ctr] {
            compute_fractal_intrinsic(
                sf::Vector2<double>(frac_tl.x + double(i) * sfractal_width, frac_tl.y),
                sf::Vector2<double>(frac_tl.x + (double(i) + 1) * sfractal_width, frac_br.y),
                sf::Vector2<int>(pixel_tl.x + i * spixel_width, pixel_tl.y),
                sf::Vector2<int>(pixel_tl.x + (i + 1) * spixel_width, pixel_br.y), iterations, fractal);
            ctr++;
            });
    }
    while (ctr < THREAD_NUMBER)
    {
    }
}