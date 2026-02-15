#ifndef SPOTIFYAUTH_H
#define SPOTIFYAUTH_H

#include <QByteArray>
#include <QString>

struct SpotifyAuth
{
    QByteArray Authorization = "";
    QByteArray ClientToken = "";
    QString PlaylistQueryHash = "";
};

#endif