#ifndef SPOTIFYAUTH_H
#define SPOTIFYAUTH_H

#include <QByteArray>

struct SpotifyAuth
{
    QByteArray Authorization = "";
    QByteArray ClientToken = "";
    QString PlaylistQueryHash = "";
};

#endif