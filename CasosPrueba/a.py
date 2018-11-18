def movieDuration(flightDuration, movieDuration):
    film_time = flightDuration - 30
    if film_time <= 0:
        return None

    film_map = {}
    for i, movie in enumerate(movieDuration):
        s = film_time-movie
        if (s) > 0 and not s in film_map:
            film_map[s] = [i]

    mins = 0
    print(film_map)

    for i, movie in enumerate(movieDuration):
        if movie in film_map and film_map[movie][0] != i:
            if (len(film_map[movie]) == 1):
                film_map[movie].append(i)
                if movie > mins:
                    mins = movie

    print(film_map)
    if mins:
        return film_map[mins]
    return None

print(movieDuration(60, [15, 15, 15, 15, 10]))
